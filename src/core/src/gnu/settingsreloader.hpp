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
#include <iostream>

#include "settings.hpp"
#include "managedtask.hpp"

namespace pipedb
{

/**
 * @brief Check in background if the configuration has changed.
 */
class SettingsReloader: public ManagedTask<>
{
public:
  static std::unique_ptr<SettingsReloader> create(const std::string& filename)
  {
    std::unique_ptr<SettingsReloader> o(new SettingsReloader(filename));
    return std::move(o);
  }

protected:
  SettingsReloader(const std::string& filename) :
      ManagedTask(), _settings()
  {
    _settings = Settings::create(filename);
  }

  virtual ~SettingsReloader()
  {
  }

  typedef typename std::unique_ptr<SettingsReloader> befriended_deleter_t;
  friend std::unique_ptr<SettingsReloader>::deleter_type;

  virtual void create_thread()
  {
    _task = std::move(std::thread([](SettingsReloader& self) mutable
    {
      std::cout << "Starting settings check loop." << std::endl;
      do
      {
        std::cout << "settings check loop iteration." << std::endl;
        auto new_crc = self._settings->checksum();
        if(self._crc != new_crc)
        {
          self._settings->load();
        }
        std::chrono::milliseconds waitDuration(1000);
        std::this_thread::sleep_for(waitDuration);
      }while(self.started());
    }, std::ref(*this)));
  }

private:
  std::unique_ptr<Settings> _settings;
  boost::optional<int32_t> _crc;
};

}
