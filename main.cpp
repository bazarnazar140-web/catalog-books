/*
    Навчальна практика
    Варіант №3 — Каталог книг

    Програма дозволяє:
    - додавати книги;
    - переглядати всі книги;
    - шукати книги за автором;
    - шукати книги за жанром;
    - оцінювати книги;
    - видаляти книги;
    - зберігати дані у файл.

    Мова програмування: C++
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Структура для зберігання інформації про книгу
struct Book {
    std::string title;
    std::string author;
    std::string genre;
    int year;
    int rating;
};

// Видаляє зайві пробіли на початку та в кінці рядка
std::string trim(const std::string& s) {
    const std::string whitespace = " \t\r\n";
    size_t start = s.find_first_not_of(whitespace);

    if (start == std::string::npos) {
        return "";
    }

    size_t end = s.find_last_not_of(whitespace);

    return s.substr(start, end - start + 1);
}

// Розбиває рядок на частини за вказаним роздільником
std::vector<std::string> splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        parts.push_back(trim(item));
    }

    return parts;
}

// Завантажує книги з текстового файлу
std::vector<Book> loadBooks(const std::string& filename) {
    std::vector<Book> books;

    std::ifstream file(filename);

    // Якщо файл не знайдено — повертаємо порожній список
    if (!file.is_open()) {
        return books;
    }

    std::string line;

    while (std::getline(file, line)) {

        // ПропускаєІ порожні рядки
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        auto parts = splitLine(line, '|');

        // Якщо формат рядка неправильний — пропускаємо його
        if (parts.size() != 5) {
            continue;
        }

        Book book;

        book.title = parts[0];
        book.author = parts[1];
        book.genre = parts[2];

        try {
            book.year = std::stoi(parts[3]);
            book.rating = std::stoi(parts[4]);
        }
        catch (...) {
            continue;
        }

        books.push_back(book);
    }

    return books;
}

// Зберігає список книг у текстовий файл
void saveBooks(const std::string& filename, const std::vector<Book>& books) {
    std::ofstream file(filename);

    for (const auto& book : books) {
        file << book.title << '|'
             << book.author << '|'
             << book.genre << '|'
             << book.year << '|'
             << book.rating << '\n';
    }
}

// Зчитує ціле число з перевіркою правильності вводу
int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;

        std::string line;

        if (!std::getline(std::cin, line)) {
            std::cout << "Помилка вводу. Спробуйте ще раз.\n";
            continue;
        }

        std::stringstream ss(line);
        int value;

        // Перевірка, чи введене число знаходиться у допустимому діапазоні
        if (ss >> value && ss.eof() &&
            value >= minValue &&
            value <= maxValue) {
            return value;
        }

        std::cout << "Неправильне число. Введіть значення від "
                  << minValue << " до "
                  << maxValue << ".\n";
    }
}

// Зчитує непорожній рядок
std::string readNonEmptyString(const std::string& prompt) {
    while (true) {
        std::cout << prompt;

        std::string value;

        if (!std::getline(std::cin, value)) {
            std::cout << "Помилка вводу. Спробуйте ще раз.\n";
            continue;
        }

        value = trim(value);

        if (!value.empty()) {
            return value;
        }

        std::cout << "Рядок не може бути порожнім. Повторіть ввід.\n";
    }
}

// Виводить інформацію про одну книгу
void printBook(int index, const Book& book) {
    std::cout << index << ". "
              << "\"" << book.title << "\" "
              << "автор: " << book.author << ", "
              << "жанр: " << book.genre << ", "
              << "рік: " << book.year << ", "
              << "рейтинг: "
              << (book.rating > 0 ?
                  std::to_string(book.rating) :
                  "не оцінено")
              << "\n";
}

// Додає нову книгу до каталогу
void addBook(std::vector<Book>& books) {
    std::cout << "\n--- Додавання книги ---\n";

    Book book;

    book.title = readNonEmptyString("Назва: ");
    book.author = readNonEmptyString("Автор: ");
    book.genre = readNonEmptyString("Жанр: ");
    book.year = readInt("Рік видання: ", 1000, 2100);

    // Початкова оцінка відсутня
    book.rating = -1;

    books.push_back(book);

    std::cout << "Книга додана.\n";
}

// Відображає всі книги каталогу
void showAllBooks(const std::vector<Book>& books) {
    std::cout << "\n--- Усі книги ---\n";

    if (books.empty()) {
        std::cout << "Каталог порожній.\n";
        return;
    }

    for (size_t i = 0; i < books.size(); ++i) {
        printBook(static_cast<int>(i + 1), books[i]);
    }
}

// Виконує пошук книги за автором або жанром
void searchByField(const std::vector<Book>& books,
                   const std::string& fieldName) {

    const std::string prompt =
        (fieldName == "автора")
        ? "Введіть автора для пошуку: "
        : "Введіть жанр для пошуку: ";

    std::string value = readNonEmptyString(prompt);

    bool found = false;

    std::cout << "\n--- Результати пошуку ---\n";

    for (size_t i = 0; i < books.size(); ++i) {

        const std::string& field =
            (fieldName == "автора")
            ? books[i].author
            : books[i].genre;

        if (field.find(value) != std::string::npos) {
            printBook(static_cast<int>(i + 1), books[i]);
            found = true;
        }
    }

    if (!found) {
        std::cout << "Нічого не знайдено за запитом.\n";
    }
}

// Дозволяє користувачу оцінити книгу
void rateBook(std::vector<Book>& books) {

    if (books.empty()) {
        std::cout << "Каталог порожній. Додайте книгу перед оцінкою.\n";
        return;
    }

    showAllBooks(books);

    int index = readInt(
        "Номер книги для оцінки: ",
        1,
        static_cast<int>(books.size())
    ) - 1;

    int rating = readInt(
        "Рейтинг (1-5): ",
        1,
        5
    );

    books[index].rating = rating;

    std::cout << "Рейтинг збережено.\n";
}

// Видаляє книгу з каталогу
void deleteBook(std::vector<Book>& books) {

    if (books.empty()) {
        std::cout << "Каталог порожній. Немає що видаляти.\n";
        return;
    }

    showAllBooks(books);

    int index = readInt(
        "Номер книги для видалення: ",
        1,
        static_cast<int>(books.size())
    ) - 1;

    books.erase(books.begin() + index);

    std::cout << "Книга видалена.\n";
}

// Виводить головне меню програми
void showMenu() {
    std::cout << "\n____| Каталог книг |____\n"
              << "1. Додати книгу\n"
              << "2. Показати всі книги\n"
              << "3. Пошук за автором\n"
              << "4. Пошук за жанром\n"
              << "5. Оцінити книгу 1\n"
              << "6. Видалити книгу\n"
              << "0. Зберегти та вийти\n";
}

// Головна функція програми
int main() {

    const std::string filename = "books.txt";

    // Завантаження даних із файлу
    auto books = loadBooks(filename);

    while (true) {

        showMenu();

        int choice = readInt("Ваш вибір: ", 0, 6);

        switch (choice) {

            case 1:
                addBook(books);
                break;

            case 2:
                showAllBooks(books);
                break;

            case 3:
                searchByField(books, "автора");
                break;

            case 4:
                searchByField(books, "жанр");
                break;

            case 5:
                rateBook(books);
                break;

            case 6:
                deleteBook(books);
                break;

            case 0:
                // Збереження даних перед завершенням програми
                saveBooks(filename, books);

                std::cout
                    << "Дані збережено у файлі '"
                    << filename
                    << "'. Вихід.\n";

                return 0;

            default:
                std::cout << "Невідомий вибір. Спробуйте ще раз.\n";
                break;
        }
    }

    return 0;
}