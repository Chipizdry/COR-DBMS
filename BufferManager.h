






/*

#pragma once
#include <unordered_map>
#include <list>
#include "Page.h"
#include "FileManager.h"
#include "ReplacementStrategy.h"
#include <memory>

class BufferManager {
public:
  //  BufferManager(size_t maxPages, const std::string& fileName);
    BufferManager(size_t maxPages, const std::string& fileName, std::unique_ptr<ReplacementStrategy> strategy);

    Page& getPage(size_t pageIndex);
    void writePage(size_t pageIndex, const Page& page);
    void flushAll();

private:
    struct Frame {
        Page page;
        bool isDirty; // ����� �� �������� �������� �� ����
    };

    std::unique_ptr<ReplacementStrategy> replacementStrategy_;

    size_t maxPages_;                              // ������������ ���������� ������� � ������
    std::unordered_map<size_t, std::list<size_t>::iterator> pageTable_; // ������ ������� � ������
    std::list<size_t> lruList_;                    // ��� LRU: ����������� ������� �������������
    std::unordered_map<size_t, Frame> frames_;     // ������ �������� � ������
    FileManager fileManager_;

    

    void evictPage(); // ��������� �������
};

*/
#pragma once
#include <unordered_map>
#include "Page.h"
#include "FileManager.h"
#include "ReplacementStrategy.h"
#include <memory>

class BufferManager {
public:
    BufferManager(size_t maxPages, const std::string& fileName, std::unique_ptr<ReplacementStrategy> strategy);

    Page& getPage(size_t pageIndex);
    void writePage(size_t pageIndex, const Page& page);
    void flushAll();

private:
    struct Frame {
        Page page;
        bool isDirty; // ����� �� �������� �������� �� ����
    };

    std::unique_ptr<ReplacementStrategy> replacementStrategy_; // ��������� ���������

    size_t maxPages_;                              // ������������ ���������� ������� � ������
    std::unordered_map<size_t, Frame> frames_;     // ������ �������� � ������
    FileManager fileManager_;

    void evictPage(); // ��������� �������
};
