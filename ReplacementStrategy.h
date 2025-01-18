



#pragma once
#include <cstddef>

class ReplacementStrategy {
public:
    virtual ~ReplacementStrategy() = default;

    // Уведомление о доступе к странице
    virtual void access(size_t pageIndex) = 0;

    // Уведомление о добавлении новой страницы
    virtual void addPage(size_t pageIndex) = 0;

    // Выбор страницы для замещения
    virtual size_t evict() = 0;
};
