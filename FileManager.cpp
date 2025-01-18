


#include "FileManager.h"
#include <fstream>
#include <stdexcept>

FileManager::FileManager(const std::string& fileName) : fileName_(fileName) {}

void FileManager::writePage(size_t pageIndex, const Page& page) {
    std::ofstream file(fileName_, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    file.seekp(pageIndex * PAGE_SIZE);
    file.write(reinterpret_cast<const char*>(page.getData().data()), PAGE_SIZE);
    file.close();
}

Page FileManager::readPage(size_t pageIndex) {
    std::ifstream file(fileName_, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    Page page;
    file.seekg(pageIndex * PAGE_SIZE);
    file.read(reinterpret_cast<char*>(page.getData().data()), PAGE_SIZE);
    file.close();
    return page;
}
