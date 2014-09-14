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

#include <memory>
#include <vector>
#include <deque>

#include <boost/utility.hpp>

#include "rwlock.hpp"

namespace pipedb
{

/**
 * @brief A pipe is a concurrent FIFO.
 * Thread-safety is provided by a RWLock.
 */
template<typename T>
class Pipe: private boost::noncopyable
{
public:
  static std::unique_ptr<Pipe> create()
  {
    std::unique_ptr < Pipe > o(new Pipe());
    return std::move(o);
  }

  void pop()
  {
    _gate.lock_for_write();
    if (!_container.empty())
    {
      _container.front().reset();
      _container.pop_front();
    }
    _gate.unlock();
  }

  void push(T* val)
  {
    _gate.lock_for_write();
    _container.emplace_back(std::shared_ptr < T > (val));
    _gate.unlock();
  }

  bool empty()
  {
    _gate.lock_for_read();
    bool res = _container.empty();
    _gate.unlock();
    return res;
  }

  /**
   * @brief utility method to avoid reallocating
   * only "reborn" the object as if it was just created
   */
  void reborn()
  {
    _gate.lock_for_write();
    while (!_container.empty())
    {
      _container.front().reset();
      _container.pop_front();
    }
    _container.clear();
    _gate.unlock();
  }

  std::weak_ptr<T> get_oldest()
  {
    if (empty())
    {
      return std::weak_ptr<T>();
    }
    _gate.lock_for_read();
    std::weak_ptr<T> res = _container.front();
    _gate.unlock();
    return res;
  }

  std::weak_ptr<T> get_newest()
  {
    if (empty())
    {
      return std::weak_ptr<T>();
    }
    _gate.lock_for_read();
    std::weak_ptr<T> res = _container.back();
    _gate.unlock();
    return res;
  }

  size_t count()
  {
    _gate.lock_for_read();
    size_t res = _container.size();
    _gate.unlock();
    return res;
  }

  /**
   * @brief Convert to a vector ordered from newest to oldest for iteration.
   * Main advantage is to iterate over all items without taking any lock.
   * A read lock is taken only while converting to ensure iterator stay valid 
   * in this method.
   * All the caller has to do is iterate over without taking any lock,
   * avoiding waits if the operation to do in each iteration is costly.
   */
  std::vector<std::weak_ptr<T> > to_weak_ptr_vector()
  {
    std::vector < std::weak_ptr<T> > res;
    res.reserve(_container.size());
    _gate.lock_for_read();
    for (auto it = _container.rbegin(); it != _container.rend(); ++it)
    {
      res.emplace_back(*it);
    }
    _gate.unlock();
    return res;
  }

private:
  Pipe() :
      _container(), _gate()
  {
    reborn();
  }

  virtual ~Pipe()
  {
    reborn();
  }

  typedef typename std::unique_ptr<Pipe<T>>::deleter_type befriended_deleter_t;
  friend befriended_deleter_t;

  /**
   * @brief container owning the allocated memory
   */
  std::deque<std::shared_ptr<T>> _container;

  /**
   * @brief Multiple read/Single writer concurrency protection
   */
  RWLock _gate;
};

}
