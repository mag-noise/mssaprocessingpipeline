#include "../src/Utilities/ModelInjector.hpp"
#include "gtest/gtest.h"

namespace Testing {
    namespace Injection {
        namespace {
            class InjectorTest : public ::testing::Test {
            
            public:
                Utils::Injector testInjector;

            protected:

                InjectorTest() {
                    testInjector = Utils::Injector("hi");
                }

                ~InjectorTest() override {
                }

            };

        }
        TEST_F(InjectorTest, GenerateInjector) {
            testInjector.HelloWorld();
            EXPECT_EQ(true, true);
        }

    }
}