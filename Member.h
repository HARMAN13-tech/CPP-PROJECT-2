#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include <vector>

// Represents a library member who can borrow books.
class Member {
public:
    Member() = default;
    Member(std::string id, std::string name, std::string email,
           std::string phone, std::string joinDate);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getJoinDate() const { return joinDate_; }
    const std::vector<std::string>& getBorrowedIsbns() const { return borrowedIsbns_; }

    void setName(const std::string& name) { name_ = name; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPhone(const std::string& phone) { phone_ = phone; }

    void borrowBook(const std::string& isbn);
    void returnBook(const std::string& isbn);
    bool hasBorrowed(const std::string& isbn) const;
    size_t borrowedCount() const { return borrowedIsbns_.size(); }

    void display() const;

    std::string toCsvRow() const;
    static Member fromCsvRow(const std::string& row);

private:
    std::string id_;
    std::string name_;
    std::string email_;
    std::string phone_;
    std::string joinDate_;
    std::vector<std::string> borrowedIsbns_;
};

#endif // MEMBER_H
