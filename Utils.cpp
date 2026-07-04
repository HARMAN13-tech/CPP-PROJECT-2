#include "Utils.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>
#include <limits>

namespace Utils {

    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            tokens.push_back(item);
        }
        // Preserve trailing empty fields (e.g. "a,b," -> {"a","b",""})
        if (!s.empty() && s.back() == delimiter) {
            tokens.push_back("");
        }
        return tokens;
    }

    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < tokens.size(); ++i) {
            result += tokens[i];
            if (i + 1 < tokens.size()) result += delimiter;
        }
        return result;
    }

    std::string todayDate() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(now, "%Y-%m-%d");
        return oss.str();
    }

    std::tm parseDate(const std::string& date) {
        std::tm tm{};
        std::istringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        return tm;
    }

    std::string addDays(const std::string& date, int days) {
        std::tm tm = parseDate(date);
        std::time_t t = std::mktime(&tm);
        t += static_cast<std::time_t>(days) * 24 * 60 * 60;
        std::tm* result = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(result, "%Y-%m-%d");
        return oss.str();
    }

    long daysBetween(const std::string& start, const std::string& end) {
        std::tm tmStart = parseDate(start);
        std::tm tmEnd = parseDate(end);
        std::time_t tStart = std::mktime(&tmStart);
        std::time_t tEnd = std::mktime(&tmEnd);
        double seconds = std::difftime(tEnd, tStart);
        return static_cast<long>(seconds / (60 * 60 * 24));
    }

    std::string generateId(const std::string& prefix, int number, int width) {
        std::ostringstream oss;
        oss << prefix << std::setw(width) << std::setfill('0') << number;
        return oss.str();
    }

    std::string toLower(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
                        [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
        std::string h = toLower(haystack);
        std::string n = toLower(needle);
        return h.find(n) != std::string::npos;
    }

    void pause() {
        std::cout << "\nPress ENTER to continue...";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    void printSeparator(char c, int length) {
        std::cout << std::string(length, c) << "\n";
    }
}
