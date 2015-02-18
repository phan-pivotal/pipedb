#pragma once
#include <gtest/gtest.h>
#include "memoryallocationchecker.hpp"

namespace pipedb_testing {
  class TestSuite: public ::testing::Test {
  protected:  
    virtual void SetUp() {
      ::testing::FLAGS_gtest_death_test_style = "threadsafe";
      MemoryChecker::resetAllocationCounter();
      beforeTestSuite();
    }
    
    virtual void TearDown() {
      EXPECT_EQ(0, MemoryChecker::getAllocationCount());
      afterTestSuite();
    }
    
    virtual void beforeTestSuite(){}
    virtual void afterTestSuite(){}
  };
  
}

