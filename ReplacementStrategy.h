



#pragma once
#include <cstddef>

class ReplacementStrategy {
public:
    virtual ~ReplacementStrategy() = default;

    // ����������� � ������� � ��������
    virtual void access(size_t pageIndex) = 0;

    // ����������� � ���������� ����� ��������
    virtual void addPage(size_t pageIndex) = 0;

    // ����� �������� ��� ���������
    virtual size_t evict() = 0;
};
