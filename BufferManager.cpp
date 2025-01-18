#include "BufferManager.h"
#include <stdexcept>

BufferManager::BufferManager(size_t maxPages, const std::string& fileName, std::unique_ptr<ReplacementStrategy> strategy)
    : maxPages_(maxPages), fileManager_(fileName), replacementStrategy_(std::move(strategy)) {
}

Page& BufferManager::getPage(size_t pageIndex) {
    // Если страница уже в памяти
    if (frames_.find(pageIndex) != frames_.end()) {
        replacementStrategy_->access(pageIndex); // Уведомляем стратегию о доступе
        return frames_[pageIndex].page;
    }

    // Если буфер заполнен, замещаем страницу
    if (frames_.size() >= maxPages_) {
        evictPage();
    }

    // Загрузка страницы с диска
    Page newPage = fileManager_.readPage(pageIndex);
    frames_[pageIndex] = { newPage, false }; // Страница не изменялась
    replacementStrategy_->addPage(pageIndex); // Уведомляем стратегию о новой странице

    return frames_[pageIndex].page;
}

void BufferManager::writePage(size_t pageIndex, const Page& page) {
    if (frames_.find(pageIndex) == frames_.end()) {
        if (frames_.size() >= maxPages_) {
            evictPage();
        }

        frames_[pageIndex] = { page, true }; // Добавляем страницу и помечаем её как изменённую
        replacementStrategy_->addPage(pageIndex); // Уведомляем стратегию о новой странице
    }
    else {
        frames_[pageIndex].page = page;
        frames_[pageIndex].isDirty = true;
        replacementStrategy_->access(pageIndex); // Уведомляем стратегию о доступе
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
    // Получаем страницу для замещения от стратегии
    size_t pageIndexToEvict = replacementStrategy_->evict();

    // Если страница была изменена, записываем её на диск
    if (frames_[pageIndexToEvict].isDirty) {
        fileManager_.writePage(pageIndexToEvict, frames_[pageIndexToEvict].page);
    }

    // Удаляем страницу из памяти
    frames_.erase(pageIndexToEvict);
}
