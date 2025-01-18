#include <iostream>
#include "FileManager.h"
#include "BufferManager.h"
#include "LRUReplacementStrategy.h"
#include "FIFOReplacementStrategy.h"
#include "ClockReplacementStrategy.h"
#include <filesystem>
#include <windows.h> // Для настройки локали на Windows

void testPageRecords(FileManager& manager) {
    std::cout << "\n=== Тест 1: Работа с записями на странице ===\n";
    Page page;

    // Вставка записей
    try {
        page.insertRecord({ 'H', 'e', 'l', 'l', 'o', '\0' });
        page.insertRecord({ 'W', 'o', 'r', 'l', 'd', '\0' });
    }
    catch (const std::exception& e) {
        std::cerr << "Error inserting record: " << e.what() << std::endl;
        return;
    }

    // Чтение записей
    for (size_t i = 0; i < page.getRecordCount(); ++i) {
        try {
            auto record = page.getRecord(i);
            std::cout << "Record " << i << ": " << reinterpret_cast<char*>(record.data()) << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading record: " << e.what() << std::endl;
        }
    }

    // Запись страницы на диск
    try {
        manager.writePage(0, page);
        std::cout << "Page written to disk successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing page: " << e.what() << std::endl;
    }
}

void testBufferManager(FileManager& manager, const std::string& strategyName, std::unique_ptr<ReplacementStrategy> strategy) {
    std::cout << "\n=== Тест 2: Работа с буферным менеджером (стратегия: " << strategyName << ") ===\n";

    BufferManager bufferManager(3, "data/database.bin", std::move(strategy));

    // Запись нескольких страниц
    for (size_t i = 0; i < 5; ++i) {
        Page page;
        std::vector<uint8_t> data = { static_cast<uint8_t>('A' + i), '\0' };
        page.insertRecord(data);

        bufferManager.writePage(i, page);
        std::cout << "Page " << i << " written with data: " << reinterpret_cast<char*>(data.data()) << std::endl;
    }

    // Чтение страниц
    for (size_t i = 0; i < 5; ++i) {
        try {
            Page& page = bufferManager.getPage(i);
            auto record = page.getRecord(0);
            std::cout << "Read page " << i << " with data: "
                << reinterpret_cast<char*>(record.data()) << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading page: " << e.what() << std::endl;
        }
    }

    // Сброс всех изменений
    bufferManager.flushAll();
}

int main() {

    // Установим кодировку консоли на UTF-8
   
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul"); // Переключение консоли на UTF-8
    // Проверка существования директории
    if (!std::filesystem::exists("data")) {
        std::filesystem::create_directory("data");
    }

    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    // Создаём менеджер файлов
    FileManager manager("data/database.bin");

    // Тест 1: Работа с записями на странице
    testPageRecords(manager);

    // Тест 2: Работа с буферным менеджером и стратегиями замещения
    testBufferManager(manager, "LRU", std::make_unique<LRUReplacementStrategy>());
    testBufferManager(manager, "FIFO", std::make_unique<FIFOReplacementStrategy>());
    testBufferManager(manager, "Clock", std::make_unique<ClockReplacementStrategy>(3)); // Передаём maxSize = 3

    return 0;
}
