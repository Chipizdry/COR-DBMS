#include "BufferManager.h"
#include <stdexcept>
#include <iostream> // ��� std::cout

BufferManager::BufferManager(size_t maxPages, const std::string& fileName, std::unique_ptr<ReplacementStrategy> strategy)
    : maxPages_(maxPages), fileManager_(fileName), replacementStrategy_(std::move(strategy)) {
}

Page& BufferManager::getPage(size_t pageIndex) {
    // ���� �������� ��� � ������
    if (frames_.find(pageIndex) != frames_.end()) {
        replacementStrategy_->access(pageIndex); // ���������� ��������� � �������
        return frames_[pageIndex].page;
    }

    // ���� ����� ��������, �������� ��������
    if (frames_.size() >= maxPages_) {
        evictPage();
    }

    // �������� �������� � �����
    Page newPage = fileManager_.readPage(pageIndex);
    frames_[pageIndex] = { newPage, false }; // �������� �� ����������
    replacementStrategy_->addPage(pageIndex); // ���������� ��������� � ����� ��������

    return frames_[pageIndex].page;
}

void BufferManager::writePage(size_t pageIndex, const Page& page) {
    if (frames_.find(pageIndex) == frames_.end()) {
        if (frames_.size() >= maxPages_) {
            evictPage();
        }

        frames_[pageIndex] = { page, true }; // ��������� �������� � �������� � ��� ���������
        replacementStrategy_->addPage(pageIndex); // ���������� ��������� � ����� ��������
    }
    else {
        frames_[pageIndex].page = page;
        frames_[pageIndex].isDirty = true;
        replacementStrategy_->access(pageIndex); // ���������� ��������� � �������
    }
}

void BufferManager::flushAll() {
    for (auto& entry : frames_) {
        if (entry.second.isDirty) {
            fileManager_.writePage(entry.first, entry.second.page);
        }
    }
    frames_.clear();
}

void BufferManager::evictPage() {
    if (frames_.empty()) {
        throw std::runtime_error("No pages to evict.");
    }

    size_t pageIndex = replacementStrategy_->evict(); // ��������� �������� �������� ��� ���������

    // ��������� ������� ��������
    auto it = frames_.find(pageIndex);
    if (it == frames_.end()) {
        throw std::runtime_error("Page to evict not found in buffer.");
    }

    std::cout << "Evicting page " << pageIndex << " from buffer.\n";

    if (it->second.isDirty) {
        fileManager_.writePage(pageIndex, it->second.page);
        std::cout << "Page " << pageIndex << " written to disk before eviction.\n";
    }

    frames_.erase(it); // ������� �������� �� ������
    std::cout << "Page " << pageIndex << " evicted.\n";
}
