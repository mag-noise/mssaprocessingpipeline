#include "../src/MSSA/MSSA.hpp"
#include "../src/SPU/SPU.hpp"

#include "gtest/gtest.h"

Utils::FlagSystem* Utils::FlagSystem::instance;
Utils::Injector* Utils::Injector::instance;
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
