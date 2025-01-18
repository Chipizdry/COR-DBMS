


#pragma once
#include <string>
#include "Page.h"

class FileManager {
public:
    FileManager(const std::string& fileName);
    void writePage(size_t pageIndex, const Page& page);
    Page readPage(size_t pageIndex);

private:
    std::string fileName_;
};
