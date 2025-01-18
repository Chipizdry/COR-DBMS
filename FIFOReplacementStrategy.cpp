


#include "FIFOReplacementStrategy.h"

void FIFOReplacementStrategy::access(size_t /*pageIndex*/) {
    // FIFO �� ����������� �������
}

void FIFOReplacementStrategy::addPage(size_t pageIndex) {
    fifoQueue_.push(pageIndex);
}

size_t FIFOReplacementStrategy::evict() {
    size_t pageIndex = fifoQueue_.front();
    fifoQueue_.pop();
    return pageIndex;
}
