#include <string>
#include "testsuite.hpp"

#include "key.hpp"

namespace pipedb_testing {
  using namespace pipedb;

  class testKey: public TestSuite {};
    
  TEST_F(testKey, Basic) {
    const char* key = "key";
    Key key1(key, 3);
    Key key2("key");
    EXPECT_TRUE(key1 == key2);
    EXPECT_TRUE(key1.referencing(key));
  }
  
}

