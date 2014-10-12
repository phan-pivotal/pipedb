#include <iostream>
#include <cassert>

#include "returnstate.hpp"
#include "rwlock.hpp"
#include "pipe.hpp"
#include "settings.hpp"

#include "settingsreloader.hpp"
#include "key.hpp"

using namespace pipedb;

Return exposed()
{
  // ...
  return Return::OK;
}

int main()
{
  assert(exposed().success());

  auto testPipe = Pipe<std::string>::create();
  testPipe->push(new std::string("1"));

  for (auto s : testPipe->to_weak_ptr_vector())
  {
    if (auto spt = s.lock())
    {
      std::cout << " s = '" << *spt << "'" << std::endl;
    }
  }

  auto testSettings = Settings::create("/tmp/toto.ini");
  testSettings->set_backend_type(1);
  assert(testSettings->save());

  std::cout << "CRC = " << testSettings->checksum() << std::endl;
  testSettings->set_backend_type(1);
  assert(testSettings->save());
  std::cout << "CRC = " << testSettings->checksum() << std::endl;
  testSettings->set_backend_type(2);
  assert(testSettings->save());
  std::cout << "CRC = " << testSettings->checksum() << std::endl;

  auto testReloadSettings = SettingsReloader::create("/tmp/toto.ini");

  testReloadSettings->start();
  std::chrono::seconds waitDuration(60);
  std::this_thread::sleep_for(waitDuration);
  testReloadSettings->stop();

  const char* key = "key";
  Key key1(key, 3);
  Key key2("key");
  assert(key1 == key2);
  assert(key1.referencing(key));

  std::cout << "" << std::endl;

  return 0;
}
