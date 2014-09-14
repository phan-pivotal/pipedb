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

#include <string.h>
#include <cstddef>
#include <string>
#include <cassert>
#include <atomic>

#include <boost/utility.hpp>

namespace pipedb
{

/**
 * @brief Defines the blocks for the API.
 */
class InputBlock: private boost::noncopyable
{
public:
  /**
   * @brief Create a block that refers to the contents of d[0,s[
   */
  InputBlock(const char* d, const size_t s) :
      _data(d), _size(s)
  {
  }

  /**
   * @brief Create a block that refers to the contents of s
   */
  InputBlock(const std::string& s) :
      _data(s.data()), _size(s.size())
  {
  }

  /**
   * @brief Move constructor for effective call from methods
   */
  InputBlock(const InputBlock&& d) :
      _data(d), _size(d._size)
  {
  }

  /**
   * @brief Return the number of bytes of the data referenced.
   */
  size_t get_size() const noexcept
  {
    return _size;
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
  bool operator==(const Block& k) const noexcept
  {
    return ((_size == k._size)
        && (referencing(k._data) || (memcmp(_data, k._data, _size) == 0)));
  }

private:
  const char* _data;
  const size_t _size;
};

}
