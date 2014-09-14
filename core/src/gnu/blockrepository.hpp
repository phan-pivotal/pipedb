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

#include <boost/utility.hpp>

#include "returnstate.h"
#include "key.hpp"
#include "inputblock.hpp"
#include "outputblock.hpp"

namespace pipedb
{

/**
 * @brief A block repository is the main interface in the API.
 *
 * All methods should be atomic and thread safe.
 */
class BlockRepository: private boost::noncopyable
{
public:
  virtual ~BlockRepository()
  {
  }

  /**
   * @brief Open the block repository.
   */
  virtual Return open() = 0;

  /**
   * @brief Check if the block repository is opened.
   */
  virtual bool opened() = 0;

  /**
   * @brief Close the block repository.
   */
  virtual void close() = 0;

  /**
   * @brief Check if the block repository is closed.
   */
  virtual bool closed() = 0;

  /**
   * @brief Put a value in the block repository.
   */
  virtual Return put(const Key&& key, const InputBlock&& value) = 0;

  /**
   * @brief Get a value from the block repository.
   */
  virtual Return get(const Key&& key, OutputBlock& value) = 0;

  /**
   * @brief Drop the key (and the associated value) from the block repository
   */
  virtual Return drop(const Key&& key) = 0;

  /**
   * @brief Check that the key exists in the block repository
   */
  virtual bool included(const Key&& key) = 0;

  /**
   * @brief Check that the key does not exists in the block repository
   */
  virtual bool excluded(const Key&& key) = 0;

protected:
  BlockRepository() = default;
};

}
