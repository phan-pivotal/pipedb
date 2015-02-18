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

namespace pipedb
{

/**
 * @brief Define the type that all calls should "return" in the exposed API.
 *
 * Here is an example of uselike :
 * Return exposed()
 * {
 *   // ...
 *   return Return::OK;
 * }
 *
 */
class Return
{
public:
  enum State
  {
    OK,
    KEY_HERETOFORE_INCLUDED,
    KEY_NOT_PRESENT,
    BACKEND_ERROR,
    INTERNAL_ERROR,
    NOT_SUPPORTED
  };

  Return(const State state) :
      _state(state)
  {
  }

  /**
   * @brief Returns true if this indicates successful operation.
   * @return A boolean as describe above.
   */
  bool success() const
  {
    return _state == OK;
  }

  /**
   * @brief Returns true if this indicate that a key is present.
   * @return A boolean as describe above.
   */
  bool key_is_present() const
  {
    return _state == OK || _state == KEY_HERETOFORE_INCLUDED;
  }

  /**
   * @brief Returns true if this indicate that a was
   * @return A boolean as describe above.
   */
  bool key_was_present() const
  {
    return _state == KEY_NOT_PRESENT;
  }

  /**
   * @brief Returns true if this indicate that an error was detected in backend used by PipeDB.
   * @return A boolean as describe above.
   */
  bool backend_error() const
  {
    return _state == BACKEND_ERROR;
  }

  /**
   * @brief Returns true if this indicate that an error was detected in PipeDB.
   * @return A boolean as describe above.
   */
  bool internal_error() const
  {
    return _state == INTERNAL_ERROR;
  }

  /**
   * @brief Returns true if this indicate that an operation is not supported in PipeDB.
   * @return A boolean as describe above.
   */
  bool not_supported() const
  {
    return _state == NOT_SUPPORTED;
  }


private:
  const State _state;
};

}
