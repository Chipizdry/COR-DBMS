



#pragma once
#include "ReplacementStrategy.h"
#include <vector>

class ClockReplacementStrategy : public ReplacementStrategy {
public:
    explicit ClockReplacementStrategy(size_t maxSize);

    void access(size_t pageIndex) override;
    void addPage(size_t pageIndex) override;
    size_t evict() override;

private:
    struct ClockEntry {
        size_t pageIndex;
        bool referenced;
    };

    std::vector<ClockEntry> clock_;
    size_t clockHand_ = 0;
    size_t maxSize_;
};
