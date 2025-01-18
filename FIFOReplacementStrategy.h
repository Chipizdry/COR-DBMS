



#pragma once
#include "ReplacementStrategy.h"
#include <queue>

class FIFOReplacementStrategy : public ReplacementStrategy {
public:
    void access(size_t pageIndex) override;
    void addPage(size_t pageIndex) override;
    size_t evict() override;

private:
    std::queue<size_t> fifoQueue_;
};
