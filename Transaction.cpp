#include "Transaction.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::string statusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::ISSUED:   return "ISSUED";
        case TransactionStatus::RETURNED: return "RETURNED";
    }
    return "UNKNOWN";
}

TransactionStatus statusFromString(const std::string& s) {
    if (s == "RETURNED") return TransactionStatus::RETURNED;
    return TransactionStatus::ISSUED;
}

Transaction::Transaction(std::string id, std::string isbn, std::string memberId,
                          std::string issueDate, std::string dueDate)
    : id_(std::move(id)), isbn_(std::move(isbn)), memberId_(std::move(memberId)),
      issueDate_(std::move(issueDate)), dueDate_(std::move(dueDate)) {}

void Transaction::markReturned(const std::string& returnDate, double fine) {
    returnDate_ = returnDate;
    fine_ = fine;
    status_ = TransactionStatus::RETURNED;
}

void Transaction::display() const {
    std::cout << std::left
               << std::setw(10) << id_
               << std::setw(12) << isbn_
               << std::setw(10) << memberId_
               << std::setw(12) << issueDate_
               << std::setw(12) << dueDate_
               << std::setw(12) << (returnDate_.empty() ? "-" : returnDate_)
               << std::setw(10) << statusToString(status_)
               << std::fixed << std::setprecision(2) << fine_
               << "\n";
}

std::string Transaction::toCsvRow() const {
    std::ostringstream oss;
    oss << id_ << "|" << isbn_ << "|" << memberId_ << "|" << issueDate_ << "|"
        << dueDate_ << "|" << returnDate_ << "|" << statusToString(status_) << "|"
        << fine_;
    return oss.str();
}

Transaction Transaction::fromCsvRow(const std::string& row) {
    std::vector<std::string> f = Utils::split(row, '|');
    Transaction t;
    if (f.size() < 8) return t;
    t.id_ = f[0];
    t.isbn_ = f[1];
    t.memberId_ = f[2];
    t.issueDate_ = f[3];
    t.dueDate_ = f[4];
    t.returnDate_ = f[5];
    t.status_ = statusFromString(f[6]);
    t.fine_ = std::stod(f[7]);
    return t;
}
