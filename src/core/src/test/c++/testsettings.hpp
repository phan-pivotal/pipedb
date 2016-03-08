#pragma once

#include <iostream>
#include <string>
#include "testsuite.hpp"

#include "settings.hpp"
#include "settingsreloader.hpp"

namespace pipedb_testing {
  using namespace pipedb;

  class testSettings: public TestSuite {};
    
  TEST_F(testSettings, BasicAndReload) {
    auto settings = Settings::create("/tmp/toto.ini");
    settings->set_backend_type(1);
    EXPECT_TRUE(settings->save());
    
    std::cout << "CRC = " << settings->checksum() << std::endl;
    settings->set_backend_type(1);
    EXPECT_TRUE(settings->save());
    std::cout << "CRC = " << settings->checksum() << std::endl;
    settings->set_backend_type(2);
    EXPECT_TRUE(settings->save());
    std::cout << "CRC = " << settings->checksum() << std::endl;

    auto reloadSettings = SettingsReloader::create("/tmp/toto.ini");
    reloadSettings->start();
    std::chrono::seconds waitDuration(5);
    std::this_thread::sleep_for(waitDuration);
    reloadSettings->stop();
  }
  
}

