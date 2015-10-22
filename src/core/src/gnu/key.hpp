/*
 * PipeDB
 * 
 * Copyright (C) 2014-2015 Jean-Manuel CABA
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

#include <string.h>
#include <cstddef>
#include <string>
#include <cassert>
#include <atomic>
#include <limits>

#include <boost/utility.hpp>

#include "chunk.hpp"

namespace pipedb
{

/**
 * @brief Defines the keys for the API. All keys are immutable.
 */
class Key: public Chunk, private boost::noncopyable
{
public:
  /**
   * @brief Create a key that refers to the contents of d[0,s[
   */
  Key(const char* d, const size_t s) :
      _data(d), _size(s)
  {
  }

  /**
   * @brief Create a key that refers to the contents of s
   */
  Key(const std::string& s) :
      _data(s.data()), _size(s.size())
  {
  }

  /**
   * @brief Move constructor for effective call from methods
   */
  Key(const Key&& k) :
      _data(k._data), _size(k._size)
  {
  }

  /**
   * @brief Return the number of bytes of the data referenced.
   */
  virtual size_t get_size() const noexcept
  {
    return _size;
  }

  /**
   * @brief Return a pointer on the data referenced.
   */
  virtual const char* get_data() const noexcept
  {
    return _data;
  }

  /**
   * @brief Is the data chunk referencing the same pointer passed as parameter d
   */
  bool referencing(const char* d) const noexcept
  {
    return d == _data;
  }

  /**
   * @brief Conversion to a std::string
   *
   * Return a string that contains the copy of the referenced data.
   */
  std::string copy_as_string() const noexcept
  {
    return std::string(_data, _size);
  }

  /**
   * @brief Equality operator
   */
  bool operator==(const Key& k) const noexcept
  {
    return ((_size == k._size)
        && (referencing(k._data) || (memcmp(_data, k._data, _size) == 0)));
  }

protected:
  template<typename T>
  static size_t partition(const T in, const size_t N)
  {
    const size_t last = std::numeric_limits < T > ::max();
    const size_t increment = last / N;
    size_t i = 0;
    size_t a = 0;
    for (; i < N; ++i)
    {
      if (in < a)
      {
        break;
      }
      a += increment;
    }
    return i - 1;
  }

  template<typename T>
  static size_t to_index(const std::string& s, const size_t N)
  {
    assert(s.size() >= sizeof(T));
    T i = 0;
    memcpy(&i, s.data(), sizeof(T));
    return partition<T>(i, N);
  }

public:
  /**
   * @brief Calculate an index to distribute the key space
   * @param n
   * @see partition
   */
  uint8_t to_8bit_index_partion(size_t N) const
  {
    return to_index<uint8_t>(copy_as_string(), N);
  }

  /**
   * @brief Calculate an index to distribute the key space.
   */
  uint16_t to_16bit_index(const size_t N) const
  {
    return to_index<uint16_t>(copy_as_string(), N);
  }

  /**
   * @brief Calculate an index to distribute the key space.
   */
  uint32_t to_32bit_index(const size_t N) const
  {
    return to_index<uint32_t>(copy_as_string(), N);
  }

  /**
   * @brief Calculate an index to distribute the key space.
   */
  uint64_t to_64bit_index(const size_t N) const
  {
    return to_index<uint64_t>(copy_as_string(), N);
  }

  uint32_t lookup_hash() const
  {
    // One at a time hash for table lookup from Bob Jenkins,
    // this hash function is under public domain
    uint32_t h = 0;

    for (size_t i = 0; i < _size; i++)
    {
      h += _data[i];
      h += (h << 10);
      h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);
    return h;
  }

private:
  const char* _data;
  const size_t _size;
};

}
