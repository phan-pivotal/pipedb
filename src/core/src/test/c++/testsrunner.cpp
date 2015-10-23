#include <chrono>
#include "testsuite.hpp"
#include "testkey.hpp"
#include "testpipe.hpp"
#include "testreturn.hpp"
#include "testsettings.hpp"

#define GTEST_COLOR

GTEST_API_ int main(int argc, char **argv) {

    std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
    int ret;
    ::testing::InitGoogleTest(&argc, argv);

    startTime = std::chrono::system_clock::now();
    ret = RUN_ALL_TESTS();
    endTime = std::chrono::system_clock::now();
    double elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()) / 1000.0;

    std::cout << "Total time elapsed: " << elapsed << " sec" << std::endl;
    return ret;
}
