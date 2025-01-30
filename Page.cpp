#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring> // ��� std::memcpy

const size_t PAGE_SIZE = 4096;
const size_t HEADER_SIZE = 128;

class Page {
public:
    Page();  // ����������� �� ���������

    void insertRecord(const std::vector<uint8_t>& record);  // ������� ������
    std::vector<uint8_t> getRecord(size_t index) const;  // ��������� ������ �� �������
    void deleteRecord(size_t index);  // �������� ������
    void updateRecord(size_t index, const std::vector<uint8_t>& newRecord);  // ���������� ������

    void compactPage();  // ��������������� �������� (����������� ������)

    std::vector<uint8_t>& getData();  // ������ � ������ ��������
    const std::vector<uint8_t>& getData() const;  // ����������� ������ � ������ ��������

    size_t getFreeSpace() const;  // ��������� ���������� ����� �� ��������
    size_t getRecordCount() const;  // ��������� ���������� ������� �� ��������

    // ����� ������ �� �����
    std::vector<uint8_t> findRecordByKey(const std::vector<uint8_t>& key);

private:
    std::vector<uint8_t> data_;  // ������ �������� (������� ��������� � ������)

    size_t getRecordOffset(size_t index) const;  // ��������� �������� ������ �� �������
    void setRecordOffset(size_t index, size_t offset);  // ��������� �������� ������
    void setRecordCount(size_t count);  // ��������� ���������� �������
};


Page::Page() : data_(PAGE_SIZE, 0) {
    setRecordCount(0);  // ������������� �������� � 0 ��������
}


size_t Page::getRecordCount() const {
    return *reinterpret_cast<const size_t*>(data_.data());  // ���������� ���������� ������� �� ���������
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
    offsets[index] = -1;  // �������� ��� ���������

    // �������� ��� ������, ������� � ���������
    for (size_t i = index + 1; i < getRecordCount(); ++i) {
        size_t offset = getRecordOffset(i);
        setRecordOffset(i - 1, offset); // �������� ��� ������
    }

    // ��������� ���������� �������
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
        // ���� ���� ��������� �����, ���������� ������
        std::vector<uint8_t> newData(PAGE_SIZE, 0);
        size_t offset = HEADER_SIZE + recordCount * sizeof(size_t);

        for (size_t i = 0; i < recordCount; ++i) {
            size_t recordOffset = getRecordOffset(i);
            size_t recordSize = (i + 1 < recordCount) ? getRecordOffset(i + 1) - recordOffset : PAGE_SIZE - recordOffset;
            std::memcpy(newData.data() + offset, data_.data() + recordOffset, recordSize);
            setRecordOffset(i, offset);
            offset += recordSize;
        }

        // ��������� ������ �������
        data_ = std::move(newData);
    }
}



std::vector<uint8_t> Page::findRecordByKey(const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < getRecordCount(); ++i) {
        std::vector<uint8_t> record = getRecord(i);
        if (std::equal(record.begin(), record.end(), key.begin())) {
            return record;  // ���������� ������, ���� ���� ������
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
