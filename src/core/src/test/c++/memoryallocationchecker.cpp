#include "memoryallocationchecker.hpp"

namespace pipedb_testing {

int_fast64_t MemoryChecker::allocation = 0;
size_t MemoryChecker::allocatedBytes = 0;

void MemoryChecker::resetAllocationCounter() {
    allocation = 0;
}

int_fast64_t MemoryChecker::getAllocationCount() {
    return allocation;
}

size_t MemoryChecker::getAllocatedBytes() {
    return allocatedBytes;
}

const int beginHeader = 0xABABABA;
const int beginBlock = 0xF0F0F0F;
const int endBlock = 0x0F0F0F0;

// global overloading of new to count number of new
// allocated bytes and detect overflow/memory corruption
void* MemoryChecker::innerNew(size_t size) {
    ++allocation;
    size_t totalSize = size + sizeof(int) * 3 + sizeof(size_t);
    void* p = malloc(size + sizeof(int) * 3 + sizeof(size_t));
    if (p == NULL) {
        // did malloc succeed?
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
    }
    else {
        bzero(p, totalSize);
        int* header = (int*) p;
        size_t* saveAllocated = (size_t*) ((char*) p + sizeof(int));
        int* begin = (int*) ((char*) p + sizeof(int) + sizeof(size_t));
        void* realPointer = (void*) ((char*) p + sizeof(int) * 2 + sizeof(size_t));
        int* end = (int*) ((char*) p + size + sizeof(int) * 2 + sizeof(size_t));

        *header = beginHeader;
        *saveAllocated = size;
        *begin = beginBlock;
        *end = endBlock;

        p = realPointer;
        allocatedBytes += size;
    }
    return p;
}

// global overloading of delete to count number of delete
void MemoryChecker::innerDelete(void* p) throw () {
    // also count deletion of NULL pointer because it's an hint of potential wrong memory handling,
    // and a potential performance amelioration by removing the calls to delete of a NULL pointer ...
    --allocation;

    if (p == NULL) // standard permit deletion of NULL pointer
        return;

    void* realPointer = (void*) ((char*) p - sizeof(int) * 2 - sizeof(size_t));

    int* header = (int*) realPointer;
    int* begin = (int*) ((char*) realPointer + sizeof(int) + sizeof(size_t));
    size_t* saveAllocated = (size_t*) ((char*) realPointer + sizeof(int));

    bool hasError = false;
    int check = 0;

    if (p != NULL) {
        check = *header;
    }
    else {
        hasError = false; // should not be here (detected by early check)
    }

    if (!hasError && (check != beginHeader)) {
        std::cerr << "Wrong block header" << std::endl;
        hasError = true;
    }

    if (!hasError && (*begin != beginBlock)) {
        std::cerr << "Wrong block begin" << std::endl;
        hasError = true;
    }

    size_t size = 0;
    if (!hasError) {
        size = *saveAllocated;
        int* end = (int*) ((char*) realPointer + size + sizeof(int) * 2 + sizeof(size_t));

        if (*end != endBlock) {
            std::cerr << "Wrong block end" << std::endl;
            hasError = true;
        }
    }

    if (hasError) {
        std::cerr << "Detected memory corruption." << std::endl;
        return;
    }

    char* toFree = (char*) realPointer;
    if (!hasError && size != 0) {
        size_t totalSize = size + sizeof(int) * 3 + sizeof(size_t);
        bzero(realPointer, totalSize);
    }
    free(toFree);
    allocatedBytes -= size;
}

}

void* operator new(size_t size) {
    return pipedb_testing::MemoryChecker::innerNew(size);
}

void* operator new[](size_t size) {
    return pipedb_testing::MemoryChecker::innerNew(size);
}

void operator delete(void *p) throw () {
  pipedb_testing::MemoryChecker::innerDelete(p);
}

void operator delete[](void *p) throw () {
  pipedb_testing::MemoryChecker::innerDelete(p);
}

