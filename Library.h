#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>
#include <optional>
#include "Book.h"
#include "Member.h"
#include "Transaction.h"

// Central class that owns all in-memory data (books, members, transactions),
// handles persistence to disk (pipe-delimited CSV files) and exposes the
// high level operations used by the CLI in main.cpp.
class Library {
public:
    explicit Library(std::string dataDirectory = "data");

    // Loads books.csv / members.csv / transactions.csv from dataDirectory_.
    // Missing files are treated as "no data yet" rather than an error.
    void loadAll();

    // Persists all in-memory data back to dataDirectory_.
    void saveAll() const;

    // --- Book operations ---
    Book& addBook(const std::string& title, const std::string& author,
                  const std::string& genre, int year, int copies);
    bool removeBook(const std::string& isbn);
    Book* findBookByIsbn(const std::string& isbn);
    std::vector<Book*> searchBooks(const std::string& keyword);
    void listBooks() const;
    size_t bookCount() const { return books_.size(); }

    // --- Member operations ---
    Member& addMember(const std::string& name, const std::string& email, const std::string& phone);
    bool removeMember(const std::string& memberId);
    Member* findMemberById(const std::string& memberId);
    std::vector<Member*> searchMembers(const std::string& keyword);
    void listMembers() const;
    size_t memberCount() const { return members_.size(); }

    // --- Circulation (borrowing / returning) ---
    // Returns an error message on failure, or std::nullopt on success.
    std::optional<std::string> issueBook(const std::string& isbn, const std::string& memberId, int loanDays = 14);
    std::optional<std::string> returnBook(const std::string& isbn, const std::string& memberId);
    void listTransactions() const;
    void listOverdue() const;

    // --- Reporting ---
    void printSummaryReport() const;
    void printMostPopularBooks(int topN = 5) const;

    static constexpr double FINE_PER_DAY = 5.0; // currency units per day overdue

private:
    std::string dataDirectory_;
    std::vector<Book> books_;
    std::vector<Member> members_;
    std::vector<Transaction> transactions_;

    int nextBookSeq_ = 1;
    int nextMemberSeq_ = 1;
    int nextTxnSeq_ = 1;

    std::string booksPath() const;
    std::string membersPath() const;
    std::string transactionsPath() const;

    void loadBooks();
    void loadMembers();
    void loadTransactions();

    void saveBooks() const;
    void saveMembers() const;
    void saveTransactions() const;

    // Scan loaded records to figure out the next free sequence number
    // for generated IDs (so restarts don't reuse old IDs).
    void recomputeSequences();
};

#endif // LIBRARY_H
