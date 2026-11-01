#include <iostream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

// Represents one book in the library.
class Book {
private:
    int id;
    string title;
    string author;
    bool issued;

public:
    Book(int bookId, string bookTitle, string bookAuthor)
        : id(bookId), title(bookTitle), author(bookAuthor), issued(false) {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool isIssued() const { return issued; }

    void issueBook() { issued = true; }
    void returnBook() { issued = false; }

    void display() const {
        cout << "ID: " << id
             << " | Title: " << title
             << " | Author: " << author
             << " | Status: " << (issued ? "Issued" : "Available") << '\n';
    }
};

// Manages the collection of books.
class Library {
private:
    vector<Book> books;

    int findBookIndex(int id) const {
        for (int i = 0; i < static_cast<int>(books.size()); ++i) {
            if (books[i].getId() == id) return i;
        }
        return -1;
    }

public:
    void addBook() {
        int id;
        string title, author;

        cout << "Enter book ID: ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findBookIndex(id) != -1) {
            cout << "A book with that ID already exists.\n";
            return;
        }

        cout << "Enter book title: ";
        getline(cin, title);
        cout << "Enter author name: ";
        getline(cin, author);

        books.emplace_back(id, title, author);
        cout << "Book added successfully!\n";
    }

    void showBooks() const {
        if (books.empty()) {
            cout << "No books have been added yet.\n";
            return;
        }

        cout << "\n--- Library Books ---\n";
        for (const Book& book : books) book.display();
    }

    void searchBook() const {
        int id;
        cout << "Enter the book ID to search: ";
        cin >> id;

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
        } else {
            books[index].display();
        }
    }

    void issueBook() {
        int id;
        cout << "Enter the book ID to issue: ";
        cin >> id;

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
        } else if (books[index].isIssued()) {
            cout << "This book is already issued.\n";
        } else {
            books[index].issueBook();
            cout << "Book issued successfully!\n";
        }
    }

    void returnBook() {
        int id;
        cout << "Enter the book ID to return: ";
        cin >> id;

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
        } else if (!books[index].isIssued()) {
            cout << "This book was not issued.\n";
        } else {
            books[index].returnBook();
            cout << "Book returned successfully!\n";
        }
    }
};

int main() {
    Library library;
    int choice;

    do {
        cout << "\n====== LIBRARY MANAGEMENT SYSTEM ======\n";
        cout << "1. Add a book\n";
        cout << "2. Display all books\n";
        cout << "3. Search for a book by ID\n";
        cout << "4. Issue a book\n";
        cout << "5. Return a book\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: library.addBook(); break;
            case 2: library.showBooks(); break;
            case 3: library.searchBook(); break;
            case 4: library.issueBook(); break;
            case 5: library.returnBook(); break;
            case 0: cout << "Thank you for using the system!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
