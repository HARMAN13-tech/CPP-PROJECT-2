#include "Library.h"
#include "Utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sys/stat.h>

Library::Library(std::string dataDirectory) : dataDirectory_(std::move(dataDirectory)) {
    // Best-effort directory creation; ignored if it already exists.
    mkdir(dataDirectory_.c_str(), 0755);
}

std::string Library::booksPath() const { return dataDirectory_ + "/books.csv"; }
std::string Library::membersPath() const { return dataDirectory_ + "/members.csv"; }
std::string Library::transactionsPath() const { return dataDirectory_ + "/transactions.csv"; }

// ---------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------

void Library::loadAll() {
    loadBooks();
    loadMembers();
    loadTransactions();
    recomputeSequences();
}

void Library::saveAll() const {
    saveBooks();
    saveMembers();
    saveTransactions();
}

void Library::loadBooks() {
    std::ifstream in(booksPath());
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;
        books_.push_back(Book::fromCsvRow(line));
    }
}

void Library::loadMembers() {
    std::ifstream in(membersPath());
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;
        members_.push_back(Member::fromCsvRow(line));
    }
}

void Library::loadTransactions() {
    std::ifstream in(transactionsPath());
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;
        transactions_.push_back(Transaction::fromCsvRow(line));
    }
}

void Library::saveBooks() const {
    std::ofstream out(booksPath(), std::ios::trunc);
    for (const auto& b : books_) out << b.toCsvRow() << "\n";
}

void Library::saveMembers() const {
    std::ofstream out(membersPath(), std::ios::trunc);
    for (const auto& m : members_) out << m.toCsvRow() << "\n";
}

void Library::saveTransactions() const {
    std::ofstream out(transactionsPath(), std::ios::trunc);
    for (const auto& t : transactions_) out << t.toCsvRow() << "\n";
}

void Library::recomputeSequences() {
    for (const auto& b : books_) {
        if (b.getIsbn().rfind("BK", 0) == 0) {
            try {
                int n = std::stoi(b.getIsbn().substr(2));
                nextBookSeq_ = std::max(nextBookSeq_, n + 1);
            } catch (...) { /* non-numeric ISBN, ignore for sequencing */ }
        }
    }
    for (const auto& m : members_) {
        if (m.getId().rfind("MB", 0) == 0) {
            try {
                int n = std::stoi(m.getId().substr(2));
                nextMemberSeq_ = std::max(nextMemberSeq_, n + 1);
            } catch (...) {}
        }
    }
    for (const auto& t : transactions_) {
        if (t.getId().rfind("TX", 0) == 0) {
            try {
                int n = std::stoi(t.getId().substr(2));
                nextTxnSeq_ = std::max(nextTxnSeq_, n + 1);
            } catch (...) {}
        }
    }
}

// ---------------------------------------------------------------------
// Books
// ---------------------------------------------------------------------

Book& Library::addBook(const std::string& title, const std::string& author,
                        const std::string& genre, int year, int copies) {
    std::string isbn = Utils::generateId("BK", nextBookSeq_++);
    books_.emplace_back(isbn, title, author, genre, year, copies);
    return books_.back();
}

bool Library::removeBook(const std::string& isbn) {
    auto it = std::find_if(books_.begin(), books_.end(),
                            [&](const Book& b) { return b.getIsbn() == isbn; });
    if (it == books_.end()) return false;
    // Refuse to delete a book that is currently on loan.
    bool onLoan = std::any_of(transactions_.begin(), transactions_.end(), [&](const Transaction& t) {
        return t.getIsbn() == isbn && t.getStatus() == TransactionStatus::ISSUED;
    });
    if (onLoan) return false;
    books_.erase(it);
    return true;
}

Book* Library::findBookByIsbn(const std::string& isbn) {
    for (auto& b : books_) {
        if (b.getIsbn() == isbn) return &b;
    }
    return nullptr;
}

std::vector<Book*> Library::searchBooks(const std::string& keyword) {
    std::vector<Book*> results;
    for (auto& b : books_) {
        if (Utils::containsIgnoreCase(b.getTitle(), keyword) ||
            Utils::containsIgnoreCase(b.getAuthor(), keyword) ||
            Utils::containsIgnoreCase(b.getGenre(), keyword) ||
            Utils::containsIgnoreCase(b.getIsbn(), keyword)) {
            results.push_back(&b);
        }
    }
    return results;
}

void Library::listBooks() const {
    if (books_.empty()) {
        std::cout << "No books in the catalog yet.\n";
        return;
    }
    std::cout << std::left
               << std::setw(12) << "ISBN"
               << std::setw(30) << "Title"
               << std::setw(20) << "Author"
               << std::setw(14) << "Genre"
               << std::setw(6) << "Year"
               << std::setw(10) << "Avail/Tot"
               << "\n";
    Utils::printSeparator();
    for (const auto& b : books_) b.display();
}

// ---------------------------------------------------------------------
// Members
// ---------------------------------------------------------------------

Member& Library::addMember(const std::string& name, const std::string& email, const std::string& phone) {
    std::string id = Utils::generateId("MB", nextMemberSeq_++);
    members_.emplace_back(id, name, email, phone, Utils::todayDate());
    return members_.back();
}

bool Library::removeMember(const std::string& memberId) {
    auto it = std::find_if(members_.begin(), members_.end(),
                            [&](const Member& m) { return m.getId() == memberId; });
    if (it == members_.end()) return false;
    if (!it->getBorrowedIsbns().empty()) return false; // must return books first
    members_.erase(it);
    return true;
}

Member* Library::findMemberById(const std::string& memberId) {
    for (auto& m : members_) {
        if (m.getId() == memberId) return &m;
    }
    return nullptr;
}

std::vector<Member*> Library::searchMembers(const std::string& keyword) {
    std::vector<Member*> results;
    for (auto& m : members_) {
        if (Utils::containsIgnoreCase(m.getName(), keyword) ||
            Utils::containsIgnoreCase(m.getEmail(), keyword) ||
            Utils::containsIgnoreCase(m.getId(), keyword)) {
            results.push_back(&m);
        }
    }
    return results;
}

void Library::listMembers() const {
    if (members_.empty()) {
        std::cout << "No members registered yet.\n";
        return;
    }
    std::cout << std::left
               << std::setw(10) << "ID"
               << std::setw(22) << "Name"
               << std::setw(28) << "Email"
               << std::setw(14) << "Phone"
               << std::setw(8) << "Loans"
               << "\n";
    Utils::printSeparator();
    for (const auto& m : members_) m.display();
}

// ---------------------------------------------------------------------
// Circulation
// ---------------------------------------------------------------------

std::optional<std::string> Library::issueBook(const std::string& isbn, const std::string& memberId, int loanDays) {
    Book* book = findBookByIsbn(isbn);
    if (!book) return "No book found with that ISBN.";
    Member* member = findMemberById(memberId);
    if (!member) return "No member found with that ID.";
    if (!book->isAvailable()) return "No available copies of that book right now.";
    if (member->hasBorrowed(isbn)) return "This member already has a copy of that book.";

    if (!book->checkOutCopy()) return "Unable to check out a copy (unexpected).";
    member->borrowBook(isbn);

    std::string txnId = Utils::generateId("TX", nextTxnSeq_++);
    std::string issueDate = Utils::todayDate();
    std::string dueDate = Utils::addDays(issueDate, loanDays);
    transactions_.emplace_back(txnId, isbn, memberId, issueDate, dueDate);

    return std::nullopt;
}

std::optional<std::string> Library::returnBook(const std::string& isbn, const std::string& memberId) {
    Member* member = findMemberById(memberId);
    if (!member) return "No member found with that ID.";
    if (!member->hasBorrowed(isbn)) return "This member has not borrowed that book.";

    Book* book = findBookByIsbn(isbn);
    if (!book) return "Book record missing (data inconsistency).";

    // Find the most recent open transaction for this isbn/member pair.
    Transaction* target = nullptr;
    for (auto it = transactions_.rbegin(); it != transactions_.rend(); ++it) {
        if (it->getIsbn() == isbn && it->getMemberId() == memberId &&
            it->getStatus() == TransactionStatus::ISSUED) {
            target = &(*it);
            break;
        }
    }
    if (!target) return "No open loan transaction found for this book/member.";

    std::string returnDate = Utils::todayDate();
    long overdueDays = Utils::daysBetween(target->getDueDate(), returnDate);
    double fine = overdueDays > 0 ? static_cast<double>(overdueDays) * FINE_PER_DAY : 0.0;

    target->markReturned(returnDate, fine);
    member->returnBook(isbn);
    book->returnCopy();

    if (fine > 0.0) {
        std::ostringstream oss;
        oss << "Returned late by " << overdueDays << " day(s). Fine due: " << std::fixed
            << std::setprecision(2) << fine;
        return oss.str(); // Not a hard error, but useful info returned via the same channel.
    }
    return std::nullopt;
}

void Library::listTransactions() const {
    if (transactions_.empty()) {
        std::cout << "No transactions recorded yet.\n";
        return;
    }
    std::cout << std::left
               << std::setw(10) << "TxnID"
               << std::setw(12) << "ISBN"
               << std::setw(10) << "MemberID"
               << std::setw(12) << "Issued"
               << std::setw(12) << "Due"
               << std::setw(12) << "Returned"
               << std::setw(10) << "Status"
               << "Fine"
               << "\n";
    Utils::printSeparator();
    for (const auto& t : transactions_) t.display();
}

void Library::listOverdue() const {
    std::string today = Utils::todayDate();
    bool any = false;
    std::cout << std::left
               << std::setw(10) << "TxnID"
               << std::setw(12) << "ISBN"
               << std::setw(10) << "MemberID"
               << std::setw(12) << "Due"
               << "DaysLate"
               << "\n";
    Utils::printSeparator();
    for (const auto& t : transactions_) {
        if (t.getStatus() != TransactionStatus::ISSUED) continue;
        long lateDays = Utils::daysBetween(t.getDueDate(), today);
        if (lateDays > 0) {
            any = true;
            std::cout << std::left
                       << std::setw(10) << t.getId()
                       << std::setw(12) << t.getIsbn()
                       << std::setw(10) << t.getMemberId()
                       << std::setw(12) << t.getDueDate()
                       << lateDays
                       << "\n";
        }
    }
    if (!any) std::cout << "No overdue loans. Nice!\n";
}

// ---------------------------------------------------------------------
// Reporting
// ---------------------------------------------------------------------

void Library::printSummaryReport() const {
    int totalCopies = 0, availableCopies = 0;
    for (const auto& b : books_) {
        totalCopies += b.getTotalCopies();
        availableCopies += b.getAvailableCopies();
    }
    int activeLoans = 0;
    double totalFinesCollected = 0.0;
    for (const auto& t : transactions_) {
        if (t.getStatus() == TransactionStatus::ISSUED) ++activeLoans;
        else totalFinesCollected += t.getFine();
    }

    Utils::printSeparator('=');
    std::cout << "LIBRARY SUMMARY REPORT\n";
    Utils::printSeparator('=');
    std::cout << "Distinct titles       : " << books_.size() << "\n";
    std::cout << "Total physical copies : " << totalCopies << "\n";
    std::cout << "Copies available now  : " << availableCopies << "\n";
    std::cout << "Registered members    : " << members_.size() << "\n";
    std::cout << "Active loans          : " << activeLoans << "\n";
    std::cout << "Total transactions    : " << transactions_.size() << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Fines collected total : " << totalFinesCollected << "\n";
    Utils::printSeparator('=');
}

void Library::printMostPopularBooks(int topN) const {
    std::map<std::string, int> borrowCounts;
    for (const auto& t : transactions_) {
        borrowCounts[t.getIsbn()]++;
    }
    std::vector<std::pair<std::string, int>> ranked(borrowCounts.begin(), borrowCounts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::cout << "Most borrowed titles:\n";
    Utils::printSeparator();
    int shown = 0;
    for (const auto& [isbn, count] : ranked) {
        if (shown >= topN) break;
        std::string title = isbn;
        for (const auto& b : books_) {
            if (b.getIsbn() == isbn) { title = b.getTitle(); break; }
        }
        std::cout << std::left << std::setw(12) << isbn << std::setw(30) << title
                   << "borrowed " << count << " time(s)\n";
        ++shown;
    }
    if (shown == 0) std::cout << "No borrowing activity yet.\n";
}
