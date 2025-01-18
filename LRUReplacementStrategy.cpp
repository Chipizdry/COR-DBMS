



#include "LRUReplacementStrategy.h"

void LRUReplacementStrategy::access(size_t pageIndex) {
    if (pageTable_.find(pageIndex) != pageTable_.end()) {
        lruList_.splice(lruList_.begin(), lruList_, pageTable_[pageIndex]);
    }
}

void LRUReplacementStrategy::addPage(size_t pageIndex) {
    lruList_.push_front(pageIndex);
    pageTable_[pageIndex] = lruList_.begin();
}

size_t LRUReplacementStrategy::evict() {
    size_t pageIndex = lruList_.back();
    lruList_.pop_back();
    pageTable_.erase(pageIndex);
    return pageIndex;
}
