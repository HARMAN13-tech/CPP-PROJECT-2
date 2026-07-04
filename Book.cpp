#include "Book.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Book::Book(std::string isbn, std::string title, std::string author,
           std::string genre, int year, int totalCopies)
    : isbn_(std::move(isbn)), title_(std::move(title)), author_(std::move(author)),
      genre_(std::move(genre)), year_(year), totalCopies_(totalCopies),
      availableCopies_(totalCopies) {}

void Book::addCopies(int count) {
    if (count <= 0) return;
    totalCopies_ += count;
    availableCopies_ += count;
}

bool Book::checkOutCopy() {
    if (availableCopies_ <= 0) return false;
    --availableCopies_;
    return true;
}

void Book::returnCopy() {
    if (availableCopies_ < totalCopies_) {
        ++availableCopies_;
    }
}

void Book::display() const {
    std::cout << std::left
               << std::setw(12) << isbn_
               << std::setw(30) << title_.substr(0, 29)
               << std::setw(20) << author_.substr(0, 19)
               << std::setw(14) << genre_.substr(0, 13)
               << std::setw(6) << year_
               << std::setw(10) << (std::to_string(availableCopies_) + "/" + std::to_string(totalCopies_))
               << "\n";
}

std::string Book::toCsvRow() const {
    std::ostringstream oss;
    oss << isbn_ << "|" << title_ << "|" << author_ << "|" << genre_ << "|"
        << year_ << "|" << totalCopies_ << "|" << availableCopies_;
    return oss.str();
}

Book Book::fromCsvRow(const std::string& row) {
    std::vector<std::string> f = Utils::split(row, '|');
    Book b;
    if (f.size() < 7) return b; // malformed row -> empty book, caller should validate
    b.isbn_ = f[0];
    b.title_ = f[1];
    b.author_ = f[2];
    b.genre_ = f[3];
    b.year_ = std::stoi(f[4]);
    b.totalCopies_ = std::stoi(f[5]);
    b.availableCopies_ = std::stoi(f[6]);
    return b;
}
