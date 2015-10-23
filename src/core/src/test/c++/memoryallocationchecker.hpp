/*
 * #%L
 * Originally from project eguan
 * Memory Tools for unit/regression testings/header
 * %%
 * Copyright (C) 2012 - 2014 Oodrive eguan.io for Nu@ge project
 * Copyrignt (C) 2015 Jean-Manuel CABA
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */
#pragma once

#include <new>
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <strings.h>
#include <iostream>

extern void* operator new(size_t size);
extern void* operator new[](size_t size);
extern void operator delete(void* ptr) throw ();
extern void operator delete[](void* ptr) throw ();

// very basic memory leak checker
// to detect regression in memory management
// count allocation ie: number of new/delete
// count number of bytes allocated,
// allocated memory is a little bigger to add markers
// at begin and end of allocated blocks
// and saving size at the beginning of the blocks
namespace pipedb_testing {



class MemoryChecker {
private:
  static int_fast64_t allocation;
  static size_t allocatedBytes;
public:
  static void* innerNew(size_t size);
  static void innerDelete(void* p) throw ();

  static void resetAllocationCounter(); // reset the counter of new/delete
  static int_fast64_t getAllocationCount(); // count number of new/delete

  static size_t getAllocatedBytes(); // get the number of bytes dynamically allocated
};

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
