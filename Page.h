



/*

#pragma once
#include <vector>
#include <cstdint>

const size_t PAGE_SIZE = 4096; // Размер страницы 4 КБ

class Page {
public:
    Page();
    std::vector<uint8_t>& getData();
    const std::vector<uint8_t>& getData() const;
    void setData(const std::vector<uint8_t>& newData);

private:
    std::vector<uint8_t> data_;
};


*/

#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring> // Для std::memcpy

const size_t PAGE_SIZE = 4096;  // Размер страницы
const size_t HEADER_SIZE = 128; // Размер заголовка страницы

class Page {
public:
    Page();

    // Методы работы с записями
    void insertRecord(const std::vector<uint8_t>& record);
    std::vector<uint8_t> getRecord(size_t index) const;
    void deleteRecord(size_t index);

    // Методы для доступа к данным страницы
    std::vector<uint8_t>& getData();
    const std::vector<uint8_t>& getData() const;

    void validateRecord(size_t index) const;

    size_t getFreeSpace() const;
    size_t getRecordCount() const; // Счётчик записей
private:
    std::vector<uint8_t> data_; // Полная страница (включая заголовок)

    // Методы для работы с заголовком
    size_t getRecordOffset(size_t index) const;
    void setRecordOffset(size_t index, size_t offset);
   // size_t getRecordCount() const;
    void setRecordCount(size_t count);
};
