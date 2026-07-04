#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

// Status of a borrowing transaction.
enum class TransactionStatus {
    ISSUED,
    RETURNED
};

std::string statusToString(TransactionStatus status);
TransactionStatus statusFromString(const std::string& s);

// Represents a single borrow/return event linking a Member to a Book.
class Transaction {
public:
    Transaction() = default;
    Transaction(std::string id, std::string isbn, std::string memberId,
                std::string issueDate, std::string dueDate);

    const std::string& getId() const { return id_; }
    const std::string& getIsbn() const { return isbn_; }
    const std::string& getMemberId() const { return memberId_; }
    const std::string& getIssueDate() const { return issueDate_; }
    const std::string& getDueDate() const { return dueDate_; }
    const std::string& getReturnDate() const { return returnDate_; }
    TransactionStatus getStatus() const { return status_; }
    double getFine() const { return fine_; }

    void markReturned(const std::string& returnDate, double fine);

    void display() const;

    std::string toCsvRow() const;
    static Transaction fromCsvRow(const std::string& row);

private:
    std::string id_;
    std::string isbn_;
    std::string memberId_;
    std::string issueDate_;
    std::string dueDate_;
    std::string returnDate_; // empty until returned
    TransactionStatus status_ = TransactionStatus::ISSUED;
    double fine_ = 0.0;
};

#endif // TRANSACTION_H
