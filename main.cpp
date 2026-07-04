// Library Management System
// A console-based application for managing a small library's catalog,
// membership and circulation (borrowing / returning books with fines).
//
// Data is persisted between runs as pipe-delimited CSV files under ./data.

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "Library.h"
#include "Utils.h"

namespace {

    void printBanner() {
        Utils::printSeparator('=');
        std::cout << "          LIBRARY MANAGEMENT SYSTEM\n";
        Utils::printSeparator('=');
    }

    void printMainMenu() {
        std::cout << "\n1.  Add Book\n"
                     "2.  List Books\n"
                     "3.  Search Books\n"
                     "4.  Remove Book\n"
                     "5.  Add Member\n"
                     "6.  List Members\n"
                     "7.  Search Members\n"
                     "8.  Remove Member\n"
                     "9.  Issue (Borrow) Book\n"
                     "10. Return Book\n"
                     "11. List Transactions\n"
                     "12. List Overdue Loans\n"
                     "13. Summary Report\n"
                     "14. Most Popular Books\n"
                     "0.  Save & Exit\n";
        std::cout << "Choose an option: ";
    }

    // Reads a full line of input safely, trimming whitespace.
    std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        return Utils::trim(line);
    }

    int readInt(const std::string& prompt) {
        while (true) {
            std::cout << prompt;
            std::string line;
            std::getline(std::cin, line);
            line = Utils::trim(line);
            try {
                return std::stoi(line);
            } catch (...) {
                std::cout << "Please enter a valid whole number.\n";
            }
        }
    }

    void handleAddBook(Library& lib) {
        std::cout << "\n-- Add a new book --\n";
        std::string title = readLine("Title: ");
        std::string author = readLine("Author: ");
        std::string genre = readLine("Genre: ");
        int year = readInt("Publication year: ");
        int copies = readInt("Number of copies: ");
        if (copies < 1) copies = 1;
        Book& b = lib.addBook(title, author, genre, year, copies);
        std::cout << "Added book with ISBN " << b.getIsbn() << "\n";
    }

    void handleListBooks(const Library& lib) {
        std::cout << "\n-- Catalog (" << lib.bookCount() << " titles) --\n";
        lib.listBooks();
    }

    void handleSearchBooks(Library& lib) {
        std::string kw = readLine("\nSearch keyword (title/author/genre/ISBN): ");
        auto results = lib.searchBooks(kw);
        if (results.empty()) {
            std::cout << "No matching books found.\n";
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
        for (auto* b : results) b->display();
    }

    void handleRemoveBook(Library& lib) {
        std::string isbn = readLine("\nISBN of book to remove: ");
        if (lib.removeBook(isbn)) {
            std::cout << "Book removed.\n";
        } else {
            std::cout << "Could not remove book (not found, or copies are currently on loan).\n";
        }
    }

    void handleAddMember(Library& lib) {
        std::cout << "\n-- Register a new member --\n";
        std::string name = readLine("Full name: ");
        std::string email = readLine("Email: ");
        std::string phone = readLine("Phone: ");
        Member& m = lib.addMember(name, email, phone);
        std::cout << "Registered member with ID " << m.getId() << "\n";
    }

    void handleListMembers(const Library& lib) {
        std::cout << "\n-- Members (" << lib.memberCount() << ") --\n";
        lib.listMembers();
    }

    void handleSearchMembers(Library& lib) {
        std::string kw = readLine("\nSearch keyword (name/email/ID): ");
        auto results = lib.searchMembers(kw);
        if (results.empty()) {
            std::cout << "No matching members found.\n";
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
        for (auto* m : results) m->display();
    }

    void handleRemoveMember(Library& lib) {
        std::string id = readLine("\nMember ID to remove: ");
        if (lib.removeMember(id)) {
            std::cout << "Member removed.\n";
        } else {
            std::cout << "Could not remove member (not found, or they still have books on loan).\n";
        }
    }

    void handleIssueBook(Library& lib) {
        std::cout << "\n-- Issue a book --\n";
        std::string isbn = readLine("Book ISBN: ");
        std::string memberId = readLine("Member ID: ");
        int loanDays = readInt("Loan period in days (e.g. 14): ");
        if (loanDays < 1) loanDays = 14;
        auto err = lib.issueBook(isbn, memberId, loanDays);
        if (err) {
            std::cout << "Could not issue book: " << *err << "\n";
        } else {
            std::cout << "Book issued successfully. Due back in " << loanDays << " day(s).\n";
        }
    }

    void handleReturnBook(Library& lib) {
        std::cout << "\n-- Return a book --\n";
        std::string isbn = readLine("Book ISBN: ");
        std::string memberId = readLine("Member ID: ");
        auto result = lib.returnBook(isbn, memberId);
        if (result) {
            // Could be either an error, or an informational late-fee message.
            std::cout << result.value() << "\n";
        } else {
            std::cout << "Book returned on time. Thank you!\n";
        }
    }

    void handleListTransactions(const Library& lib) {
        std::cout << "\n-- Transaction history --\n";
        lib.listTransactions();
    }

    void handleListOverdue(const Library& lib) {
        std::cout << "\n-- Overdue loans --\n";
        lib.listOverdue();
    }

    void handleSummaryReport(const Library& lib) {
        std::cout << "\n";
        lib.printSummaryReport();
    }

    void handlePopularBooks(const Library& lib) {
        std::cout << "\n";
        lib.printMostPopularBooks(5);
    }

    // Seeds a brand new, empty library with a few sample records so the
    // application is immediately useful/demonstrable on first run.
    void seedSampleData(Library& lib) {
        Book& b1 = lib.addBook("The Pragmatic Programmer", "Andrew Hunt", "Software", 1999, 3);
        Book& b2 = lib.addBook("Clean Code", "Robert C. Martin", "Software", 2008, 2);
        Book& b3 = lib.addBook("Dune", "Frank Herbert", "Sci-Fi", 1965, 4);
        (void)b1; (void)b2; (void)b3;

        Member& m1 = lib.addMember("Asha Patel", "asha@example.com", "555-0100");
        (void)m1;

        std::cout << "(First run detected: seeded a few sample books and one member.)\n";
    }
}

int main() {
    Library lib("data");
    lib.loadAll();

    if (lib.bookCount() == 0 && lib.memberCount() == 0) {
        seedSampleData(lib);
    }

    printBanner();

    bool running = true;
    while (running) {
        printMainMenu();
        std::string choiceLine;
        std::getline(std::cin, choiceLine);
        choiceLine = Utils::trim(choiceLine);

        int choice = -1;
        try {
            choice = std::stoi(choiceLine);
        } catch (...) {
            std::cout << "Please enter a number from the menu.\n";
            continue;
        }

        switch (choice) {
            case 1:  handleAddBook(lib); break;
            case 2:  handleListBooks(lib); break;
            case 3:  handleSearchBooks(lib); break;
            case 4:  handleRemoveBook(lib); break;
            case 5:  handleAddMember(lib); break;
            case 6:  handleListMembers(lib); break;
            case 7:  handleSearchMembers(lib); break;
            case 8:  handleRemoveMember(lib); break;
            case 9:  handleIssueBook(lib); break;
            case 10: handleReturnBook(lib); break;
            case 11: handleListTransactions(lib); break;
            case 12: handleListOverdue(lib); break;
            case 13: handleSummaryReport(lib); break;
            case 14: handlePopularBooks(lib); break;
            case 0:
                lib.saveAll();
                std::cout << "Data saved. Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "Unknown option, please try again.\n";
        }

        // Auto-save after every mutating action so data isn't lost on a crash.
        if (running) lib.saveAll();
    }

    return 0;
}
