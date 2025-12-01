#include <gtest/gtest.h>
#include <cstdlib>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    const int rc = RUN_ALL_TESTS();
}