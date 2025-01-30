#include "FileManager.h"
#include <stdexcept>
#include <iostream>

FileManager::FileManager(const std::string& fileName) : fileName_(fileName) {
    // Открытие файла в бинарном режиме для чтения и записи
    file_.open(fileName_, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file.");
    }
}

void FileManager::writePage(size_t pageIndex, const Page& page) {
    std::cout << "Writing page " << pageIndex << " to file.\n";

    if (!file_.is_open()) {
        throw std::runtime_error("File is not open for writing.");
    }

    size_t offset = pageIndex * PAGE_SIZE;
    file_.seekp(offset, std::ios::beg);

    if (!file_.good()) {
        throw std::runtime_error("Failed to seek to position in file.");
    }

    file_.write(reinterpret_cast<const char*>(page.getData().data()), PAGE_SIZE);

    if (!file_.good()) {
        throw std::runtime_error("Failed to write page to file.");
    }

    std::cout << "Page " << pageIndex << " successfully written to file.\n";
}

Page FileManager::readPage(size_t pageIndex) {
    std::cout << "Reading page " << pageIndex << " from file.\n";

    if (!file_.is_open()) {
        throw std::runtime_error("File is not open for reading.");
    }

    size_t offset = pageIndex * PAGE_SIZE;
    file_.seekg(offset, std::ios::beg);

    if (!file_.good()) {
        throw std::runtime_error("Failed to seek to position in file.");
    }

    Page page;
    file_.read(reinterpret_cast<char*>(page.getData().data()), PAGE_SIZE);

    if (!file_.good()) {
        throw std::runtime_error("Failed to read page from file.");
    }

    std::cout << "Page " << pageIndex << " successfully read from file.\n";
    return page;
}
