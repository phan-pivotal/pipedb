/*
 * PipeDB
 * 
 * Copyright (C) 2014 Jean-Manuel CABA
 * 
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
 */
#pragma once

#include <pthread.h>

namespace pipedb
{

/**
 * @brief This is a simple to use Single Write / Multiple Reader lock
 */
class RWLock
{
public:
  RWLock() :
      _impl(PTHREAD_RWLOCK_INITIALIZER)
  {
  }

  virtual ~RWLock()
  {
    unlock();
  }

  /**
   * @brief Acquire the lock in read mode
   */
  void lock_for_read()
  {
    pthread_rwlock_rdlock(&_impl);
  }

  /**
   * @brief Acquire the lock in write mode
   */
  void lock_for_write()
  {
    pthread_rwlock_wrlock(&_impl);
  }

  /**
   * @brief Release the lock
   */
  void unlock()
  {
    pthread_rwlock_unlock(&_impl);
  }

private:

  pthread_rwlock_t _impl;
};

}
