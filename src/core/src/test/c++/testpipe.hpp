#include <iostream>
#include <string>
#include "testsuite.hpp"

#include "pipe.hpp"

namespace pipedb_testing {
  using namespace pipedb;

  class testPipe: public TestSuite {};
    
  TEST_F(testPipe, Basic) {
    auto pipe = Pipe<std::string>::create();
    pipe->push(new std::string("1"));
    
    for (auto s : pipe->to_weak_ptr_vector())
      {
	if (auto spt = s.lock())
	  {
	    std::cerr << " s = '" << *spt << "'" << std::endl;
	  }
      }
  }
  
}

