#include "Member.h"
#include "Utils.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

Member::Member(std::string id, std::string name, std::string email,
               std::string phone, std::string joinDate)
    : id_(std::move(id)), name_(std::move(name)), email_(std::move(email)),
      phone_(std::move(phone)), joinDate_(std::move(joinDate)) {}

void Member::borrowBook(const std::string& isbn) {
    borrowedIsbns_.push_back(isbn);
}

void Member::returnBook(const std::string& isbn) {
    auto it = std::find(borrowedIsbns_.begin(), borrowedIsbns_.end(), isbn);
    if (it != borrowedIsbns_.end()) {
        borrowedIsbns_.erase(it);
    }
}

bool Member::hasBorrowed(const std::string& isbn) const {
    return std::find(borrowedIsbns_.begin(), borrowedIsbns_.end(), isbn) != borrowedIsbns_.end();
}

void Member::display() const {
    std::cout << std::left
               << std::setw(10) << id_
               << std::setw(22) << name_.substr(0, 21)
               << std::setw(28) << email_.substr(0, 27)
               << std::setw(14) << phone_
               << std::setw(8) << borrowedIsbns_.size()
               << "\n";
}

std::string Member::toCsvRow() const {
    std::ostringstream oss;
    oss << id_ << "|" << name_ << "|" << email_ << "|" << phone_ << "|" << joinDate_ << "|";
    for (size_t i = 0; i < borrowedIsbns_.size(); ++i) {
        oss << borrowedIsbns_[i];
        if (i + 1 < borrowedIsbns_.size()) oss << ",";
    }
    return oss.str();
}

Member Member::fromCsvRow(const std::string& row) {
    std::vector<std::string> f = Utils::split(row, '|');
    Member m;
    if (f.size() < 5) return m;
    m.id_ = f[0];
    m.name_ = f[1];
    m.email_ = f[2];
    m.phone_ = f[3];
    m.joinDate_ = f[4];
    if (f.size() >= 6 && !f[5].empty()) {
        m.borrowedIsbns_ = Utils::split(f[5], ',');
    }
    return m;
}
