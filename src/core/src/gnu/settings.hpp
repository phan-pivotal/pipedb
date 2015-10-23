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

#include <memory>
#include <vector>

#include <boost/utility.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <boost/crc.hpp>

namespace pipedb
{

class Settings: private boost::noncopyable
{
public:
  typedef uint8_t backend_t;
  
  static std::unique_ptr<Settings> create(const std::string& filename)
  {
    std::unique_ptr<Settings> o(new Settings(filename));
    return std::move(o);
  }

  bool load()
  {
    using boost::property_tree::ptree;
    try
    {
      ptree pt;

      read_ini(_filename, pt);

      _backend_type = static_cast<backend_t>(pt.get("backend_type", 0));
      _temporary_directory = pt.get < std::string > ("temporary_directory");

      for (ptree::value_type& v : pt.get_child("persistence_directories"))
      {
        _persistence_directories.emplace_back(v.second.data());
      }
      std::sort(_persistence_directories.begin(),
          _persistence_directories.end());
    }
    catch (...)
    {
      return false;
    }
    return true;
  }

  bool save()
  {
    using boost::property_tree::ptree;
    try
    {
      ptree pt;

      pt.put("backend_type", _backend_type);
      pt.put("temporary_directory", _temporary_directory);

      std::sort(_persistence_directories.begin(),
          _persistence_directories.end());
      for (const std::string& name : _persistence_directories)
      {
        pt.put("persistence_directories.directory", name);
      }

      write_ini(_filename, pt);
    }
    catch (...)
    {
      return false;
    }
    return true;
  }

  boost::optional<int32_t> checksum()
  {
    constexpr std::streamsize buffer_size = 1024;
    try
    {
      boost::crc_32_type result;
      std::ifstream ifs(_filename, std::ios_base::binary);

      if (ifs)
      {
        do
        {
          char buffer[buffer_size];

          ifs.read(buffer, buffer_size);
          result.process_bytes(buffer, ifs.gcount());
        } while (ifs);
      }
      else
      {
        return boost::none;
      }
      return boost::optional < int32_t > (result.checksum());
    }
    catch (...)
    {
      return boost::none;
    }
  }

  void set_backend_type(const int8_t type)
  {
    _backend_type = type;
  }

  int8_t get_backend_type() const
  {
    return _backend_type;
  }

  std::string get_filename() const
  {
    return _filename;
  }

private:
  Settings(const std::string& filename) :
      _filename(filename)
  {
  }

  virtual ~Settings()
  {
  }

  typedef typename std::unique_ptr<Settings> befriended_deleter_t;
  friend std::unique_ptr<Settings>::deleter_type;

  const std::string _filename;

  backend_t _backend_type;
  std::string _temporary_directory;
  std::vector<std::string> _persistence_directories;
};

}
