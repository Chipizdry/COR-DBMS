#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

// Структура для представления колонки таблицы
class Column {
public:
    std::string name;  // Имя колонки
    std::string type;  // Тип данных (например, "INT", "TEXT")
    size_t size;       // Размер в байтах (0 для TEXT)

    Column(const std::string& name, const std::string& type, size_t size)
        : name(name), type(type), size(size) {
    }
};

// Класс для представления таблицы
class Table {
public:
    std::string name;                  // Имя таблицы
    std::vector<Column> columns;       // Список колонок
    std::vector<size_t> primaryKey;    // Индексы колонок, входящих в первичный ключ

    // Конструктор
    Table(const std::string& name) : name(name) {}

    // Добавление колонки в таблицу
    void addColumn(const std::string& name, const std::string& type, size_t size) {
        columns.emplace_back(name, type, size);
    }

    // Установка первичного ключа
    void setPrimaryKey(const std::vector<size_t>& key) {
        primaryKey = key;
    }

    // Сохранение схемы таблицы в файл
    void saveSchema(std::ofstream& file) const {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing schema.");
        }

        file << name << "\n";
        file << columns.size() << "\n";
        for (const auto& column : columns) {
            file << column.name << " " << column.type << " " << column.size << "\n";
        }
        file << primaryKey.size() << "\n";
        for (size_t key : primaryKey) {
            file << key << " ";
        }
        file << "\n";
    }

    // Загрузка схемы таблицы из файла
    static Table loadSchema(std::ifstream& file) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for reading schema.");
        }

        std::string tableName;
        size_t columnCount, keyCount;
        file >> tableName >> columnCount;

        Table table(tableName);
        for (size_t i = 0; i < columnCount; ++i) {
            std::string name, type;
            size_t size;
            file >> name >> type >> size;
            table.addColumn(name, type, size);
        }

        file >> keyCount;
        std::vector<size_t> keys(keyCount);
        for (size_t i = 0; i < keyCount; ++i) {
            file >> keys[i];
        }
        table.setPrimaryKey(keys);

        return table;
    }
};

#endif // TABLE_H
