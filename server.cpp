#include "crow.h"
#include <sqlite3.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

sqlite3* database = nullptr;

// Represents one book.
class Book {
public:
    int id;
    string title;
    string author;
    bool issued;

    Book(int bookId, string bookTitle, string bookAuthor, bool bookIssued = false)
        : id(bookId), title(bookTitle), author(bookAuthor), issued(bookIssued) {}
};

// Manages books and database operations.
class Library {
public:
    bool initialize() {
        const char* sql =
            "CREATE TABLE IF NOT EXISTS books ("
            "id INTEGER PRIMARY KEY,"
            "title TEXT NOT NULL,"
            "author TEXT NOT NULL,"
            "issued INTEGER NOT NULL DEFAULT 0);";

        return sqlite3_exec(database, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
    }

    crow::json::wvalue getBooks() {
        crow::json::wvalue::list result;

        const char* sql = "SELECT id, title, author, issued FROM books ORDER BY id;";
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
            throw runtime_error("Could not read books.");

        while (sqlite3_step(statement) == SQLITE_ROW) {
            crow::json::wvalue book;

            book["id"] = sqlite3_column_int(statement, 0);
            book["title"] = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
            book["author"] = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
            book["issued"] = sqlite3_column_int(statement, 3) != 0;

            result.push_back(std::move(book));
        }

        sqlite3_finalize(statement);
        return crow::json::wvalue(std::move(result));
    }

    bool addBook(int id, const string& title, const string& author) {
        const char* sql =
            "INSERT INTO books (id, title, author, issued) VALUES (?, ?, ?, 0);";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_int(statement, 1, id);
        sqlite3_bind_text(statement, 2, title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 3, author.c_str(), -1, SQLITE_TRANSIENT);

        bool success = sqlite3_step(statement) == SQLITE_DONE;
        sqlite3_finalize(statement);

        return success;
    }

    // Returns true if the book status was changed.
    bool changeStatus(int id, bool issue) {
        const char* sql = issue
            ? "UPDATE books SET issued = 1 WHERE id = ? AND issued = 0;"
            : "UPDATE books SET issued = 0 WHERE id = ? AND issued = 1;";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_int(statement, 1, id);

        bool success = sqlite3_step(statement) == SQLITE_DONE &&
                       sqlite3_changes(database) > 0;

        sqlite3_finalize(statement);
        return success;
    }

    bool bookExists(int id) {
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(database,
            "SELECT id FROM books WHERE id = ?;", -1,
            &statement, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_int(statement, 1, id);
        bool found = sqlite3_step(statement) == SQLITE_ROW;

        sqlite3_finalize(statement);
        return found;
    }

    bool isIssued(int id) {
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(database,
            "SELECT issued FROM books WHERE id = ?;", -1,
            &statement, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_int(statement, 1, id);
        bool issued = sqlite3_step(statement) == SQLITE_ROW &&
                      sqlite3_column_int(statement, 0) != 0;

        sqlite3_finalize(statement);
        return issued;
    }
};

Library library;

crow::response errorResponse(int status, const string& message) {
    crow::json::wvalue result;
    result["message"] = message;
    return crow::response(status, result);
}

int main() {
    const char* databasePath = getenv("DATABASE_PATH");
    if (databasePath == nullptr)
        databasePath = "library.db";

    if (sqlite3_open(databasePath, &database) != SQLITE_OK) {
        cerr << "Could not open database.\n";
        return 1;
    }

    if (!library.initialize()) {
        cerr << "Could not initialize database.\n";
        sqlite3_close(database);
        return 1;
    }

    crow::SimpleApp app;

    // Get all books.
    CROW_ROUTE(app, "/api/books")
    ([&]() {
        try {
            return crow::response(200, library.getBooks().dump());
        } catch (const exception& e) {
            return errorResponse(500, e.what());
        }
    });

    // Add a new book.
    CROW_ROUTE(app, "/api/books")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& request) {
        auto body = crow::json::load(request.body);

        if (!body || !body.has("id") ||
            !body.has("title") || !body.has("author"))
            return errorResponse(400, "Please provide ID, title and author.");

        try {
            int id = stoi(body["id"].s());
            string title = body["title"].s();
            string author = body["author"].s();

            if (id <= 0 || title.empty() || author.empty())
                return errorResponse(400, "Please enter valid book details.");

            if (!library.addBook(id, title, author))
                return errorResponse(409, "A book with this ID already exists.");

            crow::json::wvalue result;
            result["message"] = "Book added successfully!";
            return crow::response(201, result.dump());
        } catch (...) {
            return errorResponse(400, "Invalid book information.");
        }
    });

    // Issue or return a book.
    CROW_ROUTE(app, "/api/books/<int>/<string>")
    .methods(crow::HTTPMethod::POST)
    ([&](int id, const string& action) {
        if (action != "issue" && action != "return")
            return errorResponse(400, "Invalid action.");

        if (!library.bookExists(id))
            return errorResponse(404, "Book not found.");

        bool issue = action == "issue";

        if (!library.changeStatus(id, issue)) {
            return errorResponse(409,
                issue ? "This book is already issued."
                      : "This book was not issued.");
        }

        crow::json::wvalue result;
        result["message"] = issue
            ? "Book issued successfully!"
            : "Book returned successfully!";

        return crow::response(200, result.dump());
    });

    int port = 8080;
    if (const char* portValue = getenv("PORT")) {
        try {
            port = stoi(portValue);
        } catch (...) {
            cerr << "Invalid PORT value.\n";
            sqlite3_close(database);
            return 1;
        }
    }

    cout << "Library Management System server starting on port "
         << port << "...\n";

    app.port(port).multithreaded().run();

    sqlite3_close(database);
    return 0;
}
