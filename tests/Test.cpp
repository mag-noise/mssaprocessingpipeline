#include "../src/MSSA/MSSA.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>

namespace my {
    namespace project {
        namespace {

            // The fixture for testing class Foo.
            class ProcessorTest : public ::testing::Test {
            protected:
                // You can remove any or all of the following functions if their bodies would
                // be empty.

                ProcessorTest() {
                    // You can do set-up work for each test here.
                }

                ~ProcessorTest() override {
                    // You can do clean-up work that doesn't throw exceptions here.
                }

                // If the constructor and destructor are not enough for setting up
                // and cleaning up each test, you can define the following methods:

                void SetUp() override {
                    // Code here will be called immediately after the constructor (right
                    // before each test).
                }

                void TearDown() override {
                    // Code here will be called immediately after each test (right
                    // before the destructor).
                }

                // Class members declared here can be used by all tests in the test suite
                // for Foo.
            };

            // Tests that Foo does Xyz.
            TEST_F(ProcessorTest, RecreatesOriginal) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;
                using Processor::MSSA;
                std::array<double, 100> input1;
                std::array<double, 100> input2;
                for_each(input1.begin(), input1.end(), [](double& n) { n = (rand() % 5); });
                for_each(input2.begin(), input2.end(), [](double& n) { n = (rand() % 5); });
                MatrixXd reconstruction = MSSA::Process(input1, input2);
                MatrixXd signal1 =
                    reconstruction.block(0, 0, MSSA::input_size, MSSA::window_size * MSSA::number_of_signals);

                int base_sum = accumulate(input1.begin(), input1.end(), 0);
                int recon_sum = nearbyint(signal1.sum());
                EXPECT_EQ(base_sum, recon_sum);
            }

        }  // namespace
    }  // namespace project
}  // namespace my

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
