#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <fstream>

using namespace std;

/* =======================
   Constants
   ======================= */
const int BOOK_ID_MIN = 10000;
const int BOOK_ID_MAX = 99999;

/* =======================
   Classes
   ======================= */
class Book {
public:
    int bookId;
    string title;
    string author;
    int copies;
};

class BorrowRecord {
public:
    int bookId;
    string borrowerName;
    string issueDate;
    bool isReturned;
};

vector<BorrowRecord> borrowHistory;

/* =======================
   Function Declarations
   ======================= */
void addBook(vector<Book>& books);
void deleteBook(vector<Book>& books);
void displayBookDetails(const Book& book);
void displayBookList(const vector<Book>& books);
void searchBook(const vector<Book>& books);
void issueBook(vector<Book>& books);
void returnBook(vector<Book>& books);

void saveToFile(const vector<Book>& books);
void loadFromFile(vector<Book>& books);
void saveBorrowHistory();
void loadBorrowHistory();
void showBorrowHistory();

bool bookExists(const vector<Book>& books, int id);
bool alreadyIssued(int bookId, const string& borrower);
bool adminLogin();

void adminMenu(vector<Book>& books);
void userMenu(vector<Book>& books);

/* =======================
   Book Operations
   ======================= */
void addBook(vector<Book>& books) {
    Book book;

    do {
        cout << "Enter 5-digit Book ID: ";
        cin >> book.bookId;
    } while (book.bookId < BOOK_ID_MIN || book.bookId > BOOK_ID_MAX);

    if (bookExists(books, book.bookId)) {
        cout << "Duplicate Book ID not allowed.\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Title: ";
    getline(cin, book.title);

    cout << "Enter Author: ";
    getline(cin, book.author);

    do {
        cout << "Enter Copies: ";
        cin >> book.copies;
    } while (book.copies <= 0);

    books.push_back(book);
    cout << "Book added successfully.\n";
}

void deleteBook(vector<Book>& books) {
    int id;
    cout << "Enter Book ID to delete: ";
    cin >> id;

    for (size_t i = 0; i < books.size(); i++) {
        if (books[i].bookId == id) {

            for (const auto& r : borrowHistory) {
                if (r.bookId == id && !r.isReturned) {
                    cout << "Book is currently issued. Cannot delete.\n";
                    return;
                }
            }

            char ch;
            cout << "Confirm delete (y/n): ";
            cin >> ch;

            if (ch == 'y' || ch == 'Y') {
                books.erase(books.begin() + i);
                saveToFile(books);
                cout << "Book deleted.\n";
            } else {
                cout << "Deletion cancelled.\n";
            }
            return;
        }
    }
    cout << "Book ID not found.\n";
}

void displayBookDetails(const Book& book) {
    cout << "\nBook ID : " << book.bookId
         << "\nTitle   : " << book.title
         << "\nAuthor  : " << book.author
         << "\nCopies  : " << book.copies << "\n";
}

void displayBookList(const vector<Book>& books) {
    if (books.empty()) {
        cout << "No books available.\n";
        return;
    }

    for (const auto& book : books)
        displayBookDetails(book);
}

void searchBook(const vector<Book>& books) {
    int id;
    cout << "Enter Book ID: ";
    cin >> id;

    for (const auto& book : books) {
        if (book.bookId == id) {
            displayBookDetails(book);
            return;
        }
    }
    cout << "Book not found.\n";
}

void issueBook(vector<Book>& books) {
    int id;
    string borrower;

    cout << "Enter Book ID: ";
    cin >> id;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Borrower Name: ";
    getline(cin, borrower);

    if (alreadyIssued(id, borrower)) {
        cout << "Borrower already has this book.\n";
        return;
    }

    for (auto& book : books) {
        if (book.bookId == id) {
            if (book.copies == 0) {
                cout << "No copies available.\n";
                return;
            }

            BorrowRecord r;
            r.bookId = id;
            r.borrowerName = borrower;

            cout << "Enter Issue Date: ";
            getline(cin, r.issueDate);

            r.isReturned = false;
            borrowHistory.push_back(r);
            book.copies--;

            saveToFile(books);
            saveBorrowHistory();

            cout << "Book issued successfully.\n";
            return;
        }
    }
    cout << "Book ID not found.\n";
}

void returnBook(vector<Book>& books) {
    int id;
    string borrower;

    cout << "Enter Book ID: ";
    cin >> id;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Borrower Name: ";
    getline(cin, borrower);

    for (auto& r : borrowHistory) {
        if (r.bookId == id && r.borrowerName == borrower && !r.isReturned) {
            r.isReturned = true;
            saveBorrowHistory();

            for (auto& book : books) {
                if (book.bookId == id) {
                    book.copies++;
                    saveToFile(books);
                    cout << "Book returned successfully.\n";
                    return;
                }
            }
        }
    }
    cout << "No matching record found.\n";
}

void showBorrowHistory() {
    if (borrowHistory.empty()) {
        cout << "No borrow history.\n";
        return;
    }

    for (const auto& r : borrowHistory) {
        cout << r.bookId << " | "
             << r.borrowerName << " | "
             << r.issueDate << " | "
             << (r.isReturned ? "Returned" : "Issued") << '\n';
    }
}

/* =======================
   File Handling
   ======================= */
void saveToFile(const vector<Book>& books) {
    ofstream file("library.txt");
    if (!file) return;

    for (const auto& b : books)
        file << b.bookId << '\n'
             << b.title << '\n'
             << b.author << '\n'
             << b.copies << '\n';
}

void loadFromFile(vector<Book>& books) {
    ifstream file("library.txt");
    if (!file) return;

    books.clear();
    Book b;
    while (file >> b.bookId) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(file, b.title);
        getline(file, b.author);
        file >> b.copies;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        books.push_back(b);
    }
}

void saveBorrowHistory() {
    ofstream file("borrow.txt");
    if (!file) return;

    for (const auto& r : borrowHistory)
        file << r.bookId << '\n'
             << r.borrowerName << '\n'
             << r.issueDate << '\n'
             << r.isReturned << '\n';
}

void loadBorrowHistory() {
    ifstream file("borrow.txt");
    if (!file) return;

    borrowHistory.clear();
    BorrowRecord r;
    while (file >> r.bookId) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(file, r.borrowerName);
        getline(file, r.issueDate);
        file >> r.isReturned;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        borrowHistory.push_back(r);
    }
}

/* =======================
   Utilities
   ======================= */
bool bookExists(const vector<Book>& books, int id) {
    for (const auto& b : books)
        if (b.bookId == id) return true;
    return false;
}

bool alreadyIssued(int bookId, const string& borrower) {
    for (const auto& r : borrowHistory)
        if (r.bookId == bookId &&
            r.borrowerName == borrower &&
            !r.isReturned)
            return true;
    return false;
}

bool adminLogin() {
    string u, p;
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;
    return (u == "admin" && p == "1234");
}

/* =======================
   Menus (SWITCH BASED)
   ======================= */
void adminMenu(vector<Book>& books) {
    int ch;
    do {
        cout << "\n1.Add 2.Delete 3.History 4.Back\nChoice: ";
        cin >> ch;

        switch (ch) {
            case 1: addBook(books); saveToFile(books); break;
            case 2: deleteBook(books); break;
            case 3: showBorrowHistory(); break;
            case 4: break;
            default: cout << "Invalid option.\n";
        }
    } while (ch != 4);
}

void userMenu(vector<Book>& books) {
    int ch;
    do {
        cout << "\n1.List 2.Search 3.Issue 4.Return 5.Exit\nChoice: ";
        cin >> ch;

        switch (ch) {
            case 1: displayBookList(books); break;
            case 2: searchBook(books); break;
            case 3: issueBook(books); break;
            case 4: returnBook(books); break;
            case 5: break;
            default: cout << "Invalid option.\n";
        }
    } while (ch != 5);
}

/* =======================
   Main
   ======================= */
int main() {
    vector<Book> books;
    loadFromFile(books);
    loadBorrowHistory();

    int ch;
    do {
        cout << "\n1.Admin 2.User 3.Exit\nChoice: ";
        cin >> ch;

        switch (ch) {
            case 1:
                if (adminLogin()) adminMenu(books);
                else cout << "Login failed.\n";
                break;
            case 2:
                userMenu(books);
                break;
            case 3:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (ch != 3);

    return 0;
}
