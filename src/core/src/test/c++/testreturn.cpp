#include "testsuite.hpp"

#include "returnstate.hpp"

namespace pipedb_testing {
  using namespace pipedb;

  class testReturn: public TestSuite {};
  
  Return exposed()
  {
    // ...
    return Return::OK;
  }
  
  TEST_F(testReturn, Basic) {
    EXPECT_TRUE(exposed().success());
  }
  
} /* namespace ibs */

