#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <ctime>

// A small collection of free helper functions used across the project:
// string manipulation, date handling and simple ID generation.
namespace Utils {

    // Trim leading/trailing whitespace from a string.
    std::string trim(const std::string& s);

    // Split a string on a delimiter, returning the resulting tokens.
    std::vector<std::string> split(const std::string& s, char delimiter);

    // Join a vector of strings using the given delimiter.
    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);

    // Return today's date formatted as YYYY-MM-DD.
    std::string todayDate();

    // Add a number of days to a YYYY-MM-DD date string and return the new date.
    std::string addDays(const std::string& date, int days);

    // Return the number of whole days between two YYYY-MM-DD dates (end - start).
    long daysBetween(const std::string& start, const std::string& end);

    // Generate a simple, incrementing, zero-padded ID with the given prefix.
    // e.g. generateId("BK", 7) -> "BK0007"
    std::string generateId(const std::string& prefix, int number, int width = 4);

    // Case-insensitive substring search: returns true if `needle` appears in `haystack`.
    bool containsIgnoreCase(const std::string& haystack, const std::string& needle);

    // Convert a string to lowercase.
    std::string toLower(const std::string& s);

    // Parse a YYYY-MM-DD date string into a std::tm structure.
    std::tm parseDate(const std::string& date);

    // Pause and wait for the user to press enter (used for simple CLI flow control).
    void pause();

    // Clear-ish separator printed between menu screens.
    void printSeparator(char c = '-', int length = 60);
}

#endif // UTILS_H
