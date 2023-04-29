#include "../../include/config.h"
#include "../../include/util/allocator.h"
#include "stdio.h"

#ifdef PRINT_MEM_STATS
U4 totalBytesAllocated = 0;
int numAllocations = 0;
int numDeAllocations = 0;
int numHeapPointersCreated = 0;
int numHeapPointersReleased = 0;
#endif

void *allocateBytes(size_t numBytes) {
#ifdef PRINT_MEM_STATS
    totalBytesAllocated += numBytes;
    numAllocations++;
#endif
    return malloc(numBytes);
}

void deAllocateBytes(void *data) {
    if (data == NULL) {
        printf("Cannot deAllocate NULL ptr!");
        exit(-1);
    }
#ifdef PRINT_MEM_STATS
    numDeAllocations++;
#endif
    free(data);
}

void printMemStats() {
#ifdef PRINT_MEM_STATS
    printf(
            "alloc %d / dealloc %d | totalBytes %lu || heapAlloc %d / heapDeAlloc %d\n",
            numAllocations, numDeAllocations,
            totalBytesAllocated,
            numHeapPointersCreated, numHeapPointersReleased
    );
#endif
}
