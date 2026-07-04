# Library Management System (C++)

A console-based Library Management System written in modern C++17. It manages
a book catalog, member registrations, and circulation (issuing/returning
books with automatic overdue fine calculation) — with all data persisted to
disk between runs.

## Features

- **Book catalog** — add, remove, search, and list books (title, author,
  genre, year, and copy counts).
- **Membership** — register, remove, search, and list library members.
- **Circulation** — issue books to members with a configurable loan period,
  return books, and automatically compute late fines.
- **Reporting** — summary statistics, overdue-loan listing, and a
  "most popular books" leaderboard based on borrow history.
- **Persistence** — all data is stored as simple pipe-delimited CSV files
  under `data/` (`books.csv`, `members.csv`, `transactions.csv`), so your
  library survives restarts with no external database required.
- **Auto-seeding** — on first run (empty `data/` directory) a few sample
  books and a member are created so you can try the app immediately.

## Project structure

```
LibraryManagementSystem/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── include/
│   ├── Book.h
│   ├── Member.h
│   ├── Transaction.h
│   ├── Library.h
│   └── Utils.h
├── src/
│   ├── Book.cpp
│   ├── Member.cpp
│   ├── Transaction.cpp
│   ├── Library.cpp
│   ├── Utils.cpp
│   └── main.cpp
└── data/            # created automatically, holds CSV data files
```

## Building

### Using CMake (recommended)

```bash
mkdir build && cd build
cmake ..
cmake --build .
./library_system
```

### Using g++ directly

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o library_system
./library_system
```

## Usage

Run the binary and follow the on-screen numbered menu:

```
1.  Add Book
2.  List Books
3.  Search Books
4.  Remove Book
5.  Add Member
6.  List Members
7.  Search Members
8.  Remove Member
9.  Issue (Borrow) Book
10. Return Book
11. List Transactions
12. List Overdue Loans
13. Summary Report
14. Most Popular Books
0.  Save & Exit
```

Data is auto-saved after every action, and again on exit, so it's safe to
close the program at any time.

### Fines

Returning a book after its due date charges a fine of **5.00 currency units
per day late** (see `Library::FINE_PER_DAY` in `include/Library.h` if you'd
like to change this).

## Design overview

- `Book`, `Member`, and `Transaction` are simple data classes, each capable
  of serializing/deserializing itself to/from a CSV row.
- `Library` is the central engine: it owns all in-memory records, handles
  file I/O, and implements the business rules (e.g. you can't delete a book
  that's currently on loan, or a member who still has outstanding loans).
- `Utils` holds small free functions for string trimming/splitting, date
  arithmetic, and ID generation, shared across the other modules.
- `main.cpp` is a thin CLI layer that only handles user I/O and delegates
  all actual logic to `Library`.

## Possible extensions

- Replace the CSV persistence layer with SQLite.
- Add unit tests (e.g. with Catch2 or GoogleTest) for `Library`'s business
  rules.
- Support reservations/holds for books that are currently checked out.
- Add a simple JSON/HTTP API layer on top of `Library` for a web front end.

## License

Released under the MIT License — see [LICENSE](LICENSE) for details.
