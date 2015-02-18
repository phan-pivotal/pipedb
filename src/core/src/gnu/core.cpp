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
#include "blockrepository.hpp"
#include "chunk.hpp"
#include "inputblock.hpp"
#include "key.hpp"
#include "ldbrepo.hpp"
#include "managedtask.hpp"
#include "outputblock.hpp"
#include "pipe.hpp"
#include "returnstate.hpp"
#include "rwlock.hpp"
#include "settings.hpp"
#include "settingsreloader.hpp"
#include "tools.hpp"
#include <string>

std::string getCoreVersion() {
  return "0.0.1";
}
