




#include "ClockReplacementStrategy.h"

ClockReplacementStrategy::ClockReplacementStrategy(size_t maxSize) : maxSize_(maxSize) {}

void ClockReplacementStrategy::access(size_t pageIndex) {
    for (auto& entry : clock_) {
        if (entry.pageIndex == pageIndex) {
            entry.referenced = true;
            break;
        }
    }
}

void ClockReplacementStrategy::addPage(size_t pageIndex) {
    if (clock_.size() < maxSize_) {
        clock_.push_back({ pageIndex, true });
    }
}

size_t ClockReplacementStrategy::evict() {
    while (true) {
        auto& entry = clock_[clockHand_];
        if (!entry.referenced) {
            size_t evictedPage = entry.pageIndex;
            clock_.erase(clock_.begin() + clockHand_);
            return evictedPage;
        }
        else {
            entry.referenced = false;
        }
        clockHand_ = (clockHand_ + 1) % clock_.size();
    }
}
