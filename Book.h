#ifndef BOOK_H
#define BOOK_H

#include <string>

// Represents a single book title in the library's catalog.
// A Book may have multiple physical copies, tracked via
// totalCopies / availableCopies.
class Book {
public:
    Book() = default;
    Book(std::string isbn, std::string title, std::string author,
         std::string genre, int year, int totalCopies);

    // --- Getters ---
    const std::string& getIsbn() const { return isbn_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    const std::string& getGenre() const { return genre_; }
    int getYear() const { return year_; }
    int getTotalCopies() const { return totalCopies_; }
    int getAvailableCopies() const { return availableCopies_; }

    // --- Setters / mutators ---
    void setTitle(const std::string& title) { title_ = title; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setGenre(const std::string& genre) { genre_ = genre; }
    void setYear(int year) { year_ = year; }
    void addCopies(int count);

    bool isAvailable() const { return availableCopies_ > 0; }
    bool checkOutCopy();   // decrements availableCopies_, returns false if none left
    void returnCopy();     // increments availableCopies_ (capped at totalCopies_)

    // Pretty single-line representation for listing in the CLI.
    void display() const;

    // Serialize / deserialize to a pipe-delimited CSV row.
    std::string toCsvRow() const;
    static Book fromCsvRow(const std::string& row);

private:
    std::string isbn_;
    std::string title_;
    std::string author_;
    std::string genre_;
    int year_ = 0;
    int totalCopies_ = 0;
    int availableCopies_ = 0;
};

#endif // BOOK_H
