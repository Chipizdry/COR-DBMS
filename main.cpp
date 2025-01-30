#include <iostream>
#include <vector>
#include <random>
#include <filesystem>
#include <windows.h>
#include <ctime>
#include "FileManager.h"
#include "BufferManager.h"
#include "Page.h"
#include "Table.h"
#include "LRUReplacementStrategy.h"
#include "FIFOReplacementStrategy.h"
#include "ClockReplacementStrategy.h"

const size_t RECORD_SIZE = 256;  // ������ ������ ������ (��������, 512 ����)

// ��������� ��������� ������ ��� ������
std::vector<uint8_t> generateRandomData() {
    std::vector<uint8_t> data(RECORD_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);  // ��������� ����� �� 0 �� 255

    for (size_t i = 0; i < RECORD_SIZE; ++i) {
        data[i] = dis(gen);
    }

    return data;
}


void testTable() {
    // ������ �������
    Table table("students");

    // ��������� �������
    table.addColumn("id", "INT", 4);
    table.addColumn("name", "TEXT", 0);  // 0 ��� ���������� �����
    table.addColumn("age", "INT", 4);

    // ������������� ��������� ����
    table.setPrimaryKey({ 0 }); // id �������� ��������� ������

    // ��������� ����� ������� � ����
    std::string schemaFile = "data/schema.db";
    std::ofstream outFile(schemaFile, std::ios::binary);
    table.saveSchema(outFile);
    outFile.close();

    std::cout << "Table schema saved to " << schemaFile << std::endl;

    // ��������� ����� ������� �� �����
    std::ifstream inFile(schemaFile, std::ios::binary);
    Table loadedTable = Table::loadSchema(inFile);
    inFile.close();

    std::cout << "Loaded table name: " << loadedTable.name << std::endl;
    std::cout << "Columns:" << std::endl;
    for (const auto& column : loadedTable.columns) {
        std::cout << "  - Name: " << column.name
            << ", Type: " << column.type
            << ", Size: " << column.size << " bytes" << std::endl;
    }
    std::cout << "Primary key column indices: ";
    for (size_t key : loadedTable.primaryKey) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
}

// ���� � ������� ����������� ��������� ������
void testPageCreationAndEvictionWithRandomData(const std::string& strategyName, std::unique_ptr<ReplacementStrategy> strategy, size_t bufferSize, size_t pageCount, size_t recordsPerPage) {
    std::cout << "\n=== ���� ���������: " << strategyName << " ===\n";

    // ������ �������� ��������
    FileManager manager("data/test_database_with_random_data.bin");

    // ������ �������� �������� � ��������� ����������
    BufferManager bufferManager(bufferSize, "data/test_database_with_random_data.bin", std::move(strategy));

    size_t currentPageIndex = 0;
    Page currentPage;

    // ��������� �������� ���������� �������
    for (size_t pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
        std::cout << "Writing records to page " << pageIndex << "...\n";
        for (size_t recordIndex = 0; recordIndex < recordsPerPage; ++recordIndex) {
            auto record = generateRandomData(); // ���������� ������ �������� 512 ����

            // ���������, ������ �� ����� �� �������� ��� ������
            if (currentPage.getFreeSpace() < record.size()) {
                std::cout << "Page " << pageIndex << " is full. Writing current page to buffer and moving to the next page.\n";
                bufferManager.writePage(currentPageIndex++, currentPage); // ��������� ������� ��������
                std::cout << "Current page index: " << currentPageIndex << std::endl;
                currentPage = Page();  // ������ ����� ��������
            }

            // ��������� ������ �� ������� ��������
            currentPage.insertRecord(record);
            std::cout << "Record " << recordIndex << " written to page " << pageIndex << ".\n";
        }
    }

    // �� �������� �������� ��������� ��������
    if (currentPage.getRecordCount() > 0) {
        std::cout << "Flushing last page...\n";
        bufferManager.writePage(currentPageIndex, currentPage);
    }

    // ������ ��������, ����� ���������, ��� ��� ��������� �������� � ������
    for (size_t pageIndex = 0; pageIndex <= currentPageIndex; ++pageIndex) {
        try {
            Page& page = bufferManager.getPage(pageIndex);
            std::cout << "Page " << pageIndex << " accessed from buffer.\n";

            // ������� ������ ������� ��� ���������� ��������
            std::cout << "Page " << pageIndex << " data:\n";
            for (size_t recordIndex = 0; recordIndex < page.getRecordCount(); ++recordIndex) {
                auto record = page.getRecord(recordIndex);
                std::cout << "Record " << recordIndex << ": ";
                for (auto byte : record) {
                    std::cout << (int)byte << " ";
                }
                std::cout << "\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error accessing page " << pageIndex << ": " << e.what() << std::endl;
        }
    }

    // ���������� ��� �������� �� ����
    bufferManager.flushAll();
    std::cout << "All pages flushed to disk.\n";
}

int main() {
    // ��������� ��������� ������� �� UTF-8
    setlocale(LC_CTYPE, "");
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul"); // ������������ ������� �� UTF-8

    try {
        // �������� ������������� ����������
        if (!std::filesystem::exists("data")) {
            std::cout << "Directory does not exist, creating it...\n";
            std::filesystem::create_directory("data");
        }

        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

        // ������������ ������ Table
        testTable();


        // �������� ��������� ��� �����
        size_t pageCount = 3;  // ���������� ������� ��� �����
        size_t bufferSize = 30;   // ������ ������
        size_t recordsPerPage = 10;  // ���������� ������� �� ��������

        // ���� ��������� FIFO � ������� ������� ������
        testPageCreationAndEvictionWithRandomData("FIFO", std::make_unique<FIFOReplacementStrategy>(), bufferSize, pageCount, recordsPerPage);

        // ���� ��������� LRU � ������� ������� ������
      //  testPageCreationAndEvictionWithRandomData("LRU", std::make_unique<LRUReplacementStrategy>(), bufferSize, pageCount, recordsPerPage);

        // ���� ��������� Clock � ������� ������� ������
     //   testPageCreationAndEvictionWithRandomData("Clock", std::make_unique<ClockReplacementStrategy>(bufferSize), bufferSize, pageCount, recordsPerPage);

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
