




#pragma once
#include "ReplacementStrategy.h"
#include <list>
#include <unordered_map>

class LRUReplacementStrategy : public ReplacementStrategy {
public:
    void access(size_t pageIndex) override;
    void addPage(size_t pageIndex) override;
    size_t evict() override;

private:
    std::list<size_t> lruList_;
    std::unordered_map<size_t, std::list<size_t>::iterator> pageTable_;
};
