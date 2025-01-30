#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring> // Для std::memcpy

const size_t PAGE_SIZE = 4096;
const size_t HEADER_SIZE = 128;

class Page {
public:
    Page();  // Конструктор по умолчанию

    void insertRecord(const std::vector<uint8_t>& record);  // Вставка записи
    std::vector<uint8_t> getRecord(size_t index) const;  // Получение записи по индексу
    void deleteRecord(size_t index);  // Удаление записи
    void updateRecord(size_t index, const std::vector<uint8_t>& newRecord);  // Обновление записи

    void compactPage();  // Компактирование страницы (перемещение данных)

    std::vector<uint8_t>& getData();  // Доступ к данным страницы
    const std::vector<uint8_t>& getData() const;  // Константный доступ к данным страницы

    size_t getFreeSpace() const;  // Получение свободного места на странице
    size_t getRecordCount() const;  // Получение количества записей на странице

    // Поиск записи по ключу
    std::vector<uint8_t> findRecordByKey(const std::vector<uint8_t>& key);

private:
    std::vector<uint8_t> data_;  // Полная страница (включая заголовок и записи)

    size_t getRecordOffset(size_t index) const;  // Получение смещения записи по индексу
    void setRecordOffset(size_t index, size_t offset);  // Установка смещения записи
    void setRecordCount(size_t count);  // Установка количества записей
};


Page::Page() : data_(PAGE_SIZE, 0) {
    setRecordCount(0);  // Инициализация страницы с 0 записями
}


size_t Page::getRecordCount() const {
    return *reinterpret_cast<const size_t*>(data_.data());  // Извлечение количества записей из заголовка
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
    size_t usedSpace = HEADER_SIZE + recordCount * sizeof(size_t);
    if (recordCount > 0) {
        size_t lastOffset = getRecordOffset(recordCount - 1);
        usedSpace += PAGE_SIZE - lastOffset;
    }
    return PAGE_SIZE - usedSpace;
}

void Page::insertRecord(const std::vector<uint8_t>& record) {
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
}

std::vector<uint8_t> Page::getRecord(size_t index) const {
    size_t offset = getRecordOffset(index);
    size_t nextOffset = (index + 1 < getRecordCount()) ? getRecordOffset(index + 1) : PAGE_SIZE;
    size_t recordSize = nextOffset - offset;
    std::vector<uint8_t> record(recordSize);
    std::memcpy(record.data(), data_.data() + offset, recordSize);
    return record;
}


void Page::deleteRecord(size_t index) {
    if (index >= getRecordCount()) {
        throw std::out_of_range("Invalid record index");
    }

    size_t* offsets = reinterpret_cast<size_t*>(data_.data() + sizeof(size_t));
    offsets[index] = -1;  // Помечаем как удаленную

    // Сдвигаем все записи, начиная с удаленной
    for (size_t i = index + 1; i < getRecordCount(); ++i) {
        size_t offset = getRecordOffset(i);
        setRecordOffset(i - 1, offset); // Сдвигаем все записи
    }

    // Уменьшаем количество записей
    setRecordCount(getRecordCount() - 1);
}





void Page::updateRecord(size_t index, const std::vector<uint8_t>& newRecord) {
    if (newRecord.size() > getFreeSpace()) {
        throw std::runtime_error("Not enough free space to update the record");
    }

    size_t offset = getRecordOffset(index);
    std::memcpy(data_.data() + offset, newRecord.data(), newRecord.size());
}



void Page::compactPage() {
    size_t recordCount = getRecordCount();
    size_t freeSpace = PAGE_SIZE - HEADER_SIZE - recordCount * sizeof(size_t);

    if (freeSpace > 0) {
        // Если есть свободное место, перемещаем данные
        std::vector<uint8_t> newData(PAGE_SIZE, 0);
        size_t offset = HEADER_SIZE + recordCount * sizeof(size_t);

        for (size_t i = 0; i < recordCount; ++i) {
            size_t recordOffset = getRecordOffset(i);
            size_t recordSize = (i + 1 < recordCount) ? getRecordOffset(i + 1) - recordOffset : PAGE_SIZE - recordOffset;
            std::memcpy(newData.data() + offset, data_.data() + recordOffset, recordSize);
            setRecordOffset(i, offset);
            offset += recordSize;
        }

        // Переносим данные обратно
        data_ = std::move(newData);
    }
}



std::vector<uint8_t> Page::findRecordByKey(const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < getRecordCount(); ++i) {
        std::vector<uint8_t> record = getRecord(i);
        if (std::equal(record.begin(), record.end(), key.begin())) {
            return record;  // Возвращаем запись, если ключ совпал
        }
    }
    throw std::runtime_error("Record with the given key not found");
}

std::vector<uint8_t>& Page::getData() {
    return data_;
}

const std::vector<uint8_t>& Page::getData() const {
    return data_;
}
