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

#include <boost/utility.hpp>

#include "Chunk.hpp"

namespace pipedb
{

/**
 * @brief Defines the output blocks for the API.
 */
class OutputBlock: private boost::noncopyable, public Chunk
{
public:
  /**
   * @brief Create an empty output block.
   */
  OutputBlock() = default;

  /**
   * @brief Move constructor for effective call from methods
   */
  OutputBlock(const OutputBlock&& d) :
      _data(d._data)
  {
  }

  /**
   * @brief Return the number of bytes of the data referenced.
   */
  virtual size_t get_size() const noexcept
  {
    return _data.size();
  }

  /**
   * @brief Return a pointer on the data referenced.
   */
  virtual const char* get_data() const noexcept
  {
    return _data.data();
  }

  /**
   * @brief Change internal raw data.
   */
  void set_data(const char* d, const size_t s)
  {
    _data.assign(d, s);
  }

  /**
   * @brief Change internal raw data.
   */
  void set_data(const std::string& s)
  {
    _data.assign(s.data(), s.size());
  }

  /**
   * @brief Conversion to a std::string
   *
   * Return a string that contains the copy of the referenced data.
   */
  std::string copy_as_string() const noexcept
  {
    return _data;
  }

  /**
   * @brief Equality operator
   */
  bool operator==(const OutputBlock& d) const noexcept
  {
    return _data == d._data;
  }

private:
  std::string _data;
};

}
