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

#include <thread>
#include <atomic>
#include <system_error>

#include <boost/utility.hpp>

namespace pipedb
{

class Settings;

/**
 * @brief A task executed in a separate thread.
 * The task can be executed N times only.
 */
template<size_t N = 1>
class ManagedTask: private boost::noncopyable
{
public:
  /**
   * @brief Create and start the task in a separate thread. (if not already done)
   * If the was has run already
   */
  void start()
  {
    if ((_run_counter < N) && !_stopping && (_started.exchange(true) == false))
    {
      ++_run_counter;
      create_thread();
    }
  }

  /**
   * @return If the task is stopped.
   */
  bool started() const
  {
    return _started;
  }

  /**
   * @brief Join and stop the task's separate thread.
   */
  void stop()
  {
    if (_started && (_stopping.exchange(true) == false))
    {
      _started = false;
      if (_task.joinable())
      {
        try
        {
          _task.join();
        }
        catch (...)
        {
          try
          {
            _task.detach();
          }
          catch (...)
          {
          }
        }
      }
      _stopping = false;
    }
  }

  /**
   * @return If the task is stopped.
   */
  bool stopped() const
  {
    return !_started;
  }

protected:
  ManagedTask() :
      _task(), _started(false), _stopping(false), _run_counter(0)
  {
  }

  /**
   * @brief At destruction stop task if not already done.
   * It's recommended to stop before destruction as
   * this can lead to undefined behavior.
   */
  virtual ~ManagedTask()
  {
    stop();
  }

  /**
   * @brief  Initialize task member by creating thread
   */
  virtual void create_thread() = 0;

  std::thread _task;
private:

  typedef typename std::unique_ptr<Settings> befriended_deleter_t;
  friend std::unique_ptr<Settings>::deleter_type;

  std::atomic<bool> _started;
  std::atomic<bool> _stopping;
  std::atomic<uint8_t> _run_counter;
};

}
