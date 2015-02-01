#pragma once
#include <gtest/gtest.h>
#include "memoryChecker.h"

namespace pipedb_testing {
  class TestSuite: public ::testing::Test {
  protected:  
    virtual void SetUp() {
      ::testing::FLAGS_gtest_death_test_style = "threadsafe";
      resetAllocationCounter();
      beforeTestSuite();
    }
    
    virtual void TearDown() {
      EXPECT_EQ(0, getAllocationCount());
      afterTestSuite();
    }
    
    virtual void beforeTestSuite(){}
    virtual void afterTestSuite(){}
  };
  
}

