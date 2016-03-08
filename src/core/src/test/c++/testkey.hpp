#include <string>
#include "testsuite.hpp"

#include "key.hpp"

namespace pipedb_testing {
  using namespace pipedb;

  class testKey: public TestSuite {};
    
  TEST_F(testKey, Basic) {
    std::string key_str("key");
    const char* key = "key";
    Key key1(key, 3);
    Key key2(key_str);

    EXPECT_TRUE(key1.get_size() == key2.get_size());
    EXPECT_FALSE(key1.referencing(key2.get_data()));
    for(size_t i=0; i<3; ++i) {
      EXPECT_EQ(key1.get_data()[i], key2.get_data()[i]);
    }
    EXPECT_TRUE(key1 == key2);
    EXPECT_TRUE(key2 == key1);
    EXPECT_TRUE(key1.referencing(key));
  }
  
}

