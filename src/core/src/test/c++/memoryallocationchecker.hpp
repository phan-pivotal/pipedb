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

}
extern void* operator new(size_t size);
extern void* operator new[](size_t size);
extern void operator delete(void* ptr) throw ();
extern void operator delete[](void* ptr) throw ();
