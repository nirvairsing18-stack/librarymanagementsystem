# Library Management System (C++ OOP)

A small, menu-driven Library Management System built with C++ and Object-Oriented Programming. It is designed as a simple demonstration project for a college assignment.

## Features

- Add a book with an ID, title, and author
- Display all books
- Search for a book by ID
- Issue an available book
- Return an issued book
- Prevent duplicate book IDs and issuing an already-issued book

## OOP concepts used

- **Classes and objects:** `Book` represents a book; `Library` manages books.
- **Encapsulation:** Book data is private and accessed through public methods.
- **Constructors:** The `Book` constructor initializes each book.
- **Member functions:** Functions organize the program's actions.

## Requirements

A C++ compiler that supports C++17, such as GCC, Clang, or Microsoft Visual C++.

## Run locally

### Linux / macOS

```bash
g++ -std=c++17 main.cpp -o library
./library
```

### Windows (MinGW)

```bash
g++ -std=c++17 main.cpp -o library.exe
library.exe
```

## Important note

This is a demonstration version. Books are stored in memory, so the list resets when the program closes. GitHub hosts the source code but does not run this interactive C++ console program directly on a repository page. Download or clone the repository and compile it to run the program.

## Upload to GitHub

1. Create a repository named `LibraryManagementSystem` on GitHub.
2. Upload `main.cpp`, `README.md`, `.gitignore`, and `CMakeLists.txt` (or use the Git commands below).
3. Commit the files to the `main` branch.

Using Git from the project folder:

```bash
git init
git add .
git commit -m "Add C++ Library Management System"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/LibraryManagementSystem.git
git push -u origin main
```

Replace `YOUR-USERNAME` with your GitHub username. If you already connected this folder to a repository, do not run `git remote add origin` again; use the existing remote.
