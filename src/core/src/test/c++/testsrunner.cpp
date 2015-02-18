#include <chrono>
#include "testsuite.hpp"

#define GTEST_COLOR

GTEST_API_ int main(int argc, char **argv) {

    std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
    int ret;
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();

    startTime = std::chrono::system_clock::now();
    ret = RUN_ALL_TESTS();
    endTime = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;

    std::cout << "Total time elapsed: " << elapsed << " sec" << std::endl;

    return ret;
}
