
#include <iostream>   // Для std::cout, std::endl
#include <vector>     // Для std::vector
#include <cstdint>    // Для uint8_t и size_t
#include <cstring>    // Для std::memcpy
#include <stdexcept>  // Для std::runtime_error и std::out_of_range
#include "Page.h"
Page::Page() : data_(PAGE_SIZE, 0) {
    setRecordCount(0); // Изначально записей нет
}

size_t Page::getRecordCount() const {
    return *reinterpret_cast<const size_t*>(data_.data());
}

void Page::setRecordCount(size_t count) {
    *reinterpret_cast<size_t*>(data_.data()) = count;
}

size_t Page::getRecordOffset(size_t index) const {
    if (index >= getRecordCount()) {
        throw std::out_of_range("Invalid record index");
    }
    const size_t* offsets = reinterpret_cast<const size_t*>(data_.data() + sizeof(size_t));
    return offsets[index];
}

void Page::setRecordOffset(size_t index, size_t offset) {
    size_t* offsets = reinterpret_cast<size_t*>(data_.data() + sizeof(size_t));
    offsets[index] = offset;
}


size_t Page::getFreeSpace() const {
    size_t recordCount = getRecordCount();
    size_t usedSpace = HEADER_SIZE + recordCount * sizeof(size_t); // Заголовок и смещения

    // Учитываем размеры всех записей
    if (recordCount > 0) {
        size_t lastOffset = getRecordOffset(recordCount - 1);
        usedSpace += PAGE_SIZE - lastOffset;
    }

    if (usedSpace > PAGE_SIZE) {
        throw std::runtime_error("Used space exceeds page size");
    }

    return PAGE_SIZE - usedSpace;
}




void Page::insertRecord(const std::vector<uint8_t>& record) {
    if (record.size() > getFreeSpace()) {
        throw std::runtime_error("Record exceeds page size");
    }

    size_t recordCount = getRecordCount();
    size_t offset = PAGE_SIZE - getFreeSpace();

    if (offset + record.size() > PAGE_SIZE) {
        throw std::runtime_error("Calculated offset exceeds page size");
    }

    std::memcpy(data_.data() + offset, record.data(), record.size());


    std::cout << "Free space: " << getFreeSpace()
        << ", record size: " << record.size() << std::endl;
    setRecordOffset(recordCount, offset);
    setRecordCount(recordCount + 1);
}



/*
void Page::insertRecord(const std::vector<uint8_t>& record) {
    std::cout << "Inserting record of size: " << record.size() << ", Free space: " << getFreeSpace() << std::endl;

    if (record.size() > getFreeSpace()) {
        throw std::runtime_error("Record exceeds page size");
    }

    size_t recordCount = getRecordCount();
    size_t offset = (recordCount > 0) ? getRecordOffset(recordCount - 1) + record.size() : HEADER_SIZE;

    if (offset + record.size() > PAGE_SIZE) {
        throw std::runtime_error("Calculated offset exceeds page size");
    }

    std::memcpy(data_.data() + offset, record.data(), record.size());

    setRecordOffset(recordCount, offset);
    setRecordCount(recordCount + 1);

    std::cout << "Record inserted. New free space: " << getFreeSpace() << std::endl;
}

*/

/*

std::vector<uint8_t> Page::getRecord(size_t index) const {
    size_t offset = getRecordOffset(index);
    size_t nextOffset = (index + 1 < getRecordCount()) ? getRecordOffset(index + 1) : PAGE_SIZE;

    size_t recordSize = nextOffset - offset;
    std::vector<uint8_t> record(recordSize);
    std::memcpy(record.data(), data_.data() + offset, recordSize);

    return record;
}
*/

std::vector<uint8_t> Page::getRecord(size_t index) const {
    size_t offset = getRecordOffset(index);
    size_t nextOffset = (index + 1 < getRecordCount()) ? getRecordOffset(index + 1) : PAGE_SIZE;

    // Добавим проверку на корректность offset и nextOffset
    if (offset >= nextOffset || nextOffset > PAGE_SIZE) {
        throw std::runtime_error("Invalid record boundaries");
    }

    size_t recordSize = nextOffset - offset;
    std::vector<uint8_t> record(recordSize);

    std::memcpy(record.data(), data_.data() + offset, recordSize);
    return record;
}

void Page::validateRecord(size_t index) const {
    if (index >= getRecordCount()) {
        throw std::runtime_error("Invalid record index during validation");
    }

    size_t offset = getRecordOffset(index);
    size_t nextOffset = (index + 1 < getRecordCount()) ? getRecordOffset(index + 1) : PAGE_SIZE;

    if (offset >= nextOffset || nextOffset > PAGE_SIZE) {
        throw std::runtime_error("Corrupted record boundaries");
    }
}




void Page::deleteRecord(size_t index) {
    throw std::runtime_error("Delete record not implemented yet");
}

std::vector<uint8_t>& Page::getData() {
    return data_;
}

const std::vector<uint8_t>& Page::getData() const {
    return data_;
}