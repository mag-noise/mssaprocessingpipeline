#include "../src/MSSA/MSSA.hpp"
#include "../src/CPU/CPU.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>

namespace Testing {
    namespace PreProcessingTest {
        namespace {

            class PreProcessorTest : public ::testing::Test {
            public:
                std::string test_csv;
            protected:

                // You can remove any or all of the following functions if their bodies would
                // be empty.

                PreProcessorTest() {
                    // You can do set-up work for each test here.
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                }

                ~PreProcessorTest() override {
                    // You can do clean-up work that doesn't throw exceptions here.
                }

            };


            TEST_F(PreProcessorTest, CorrectlyReadsCSVFile) {
                using CentralProcessingUnit::MSSAProcessingUnit;
                using Processor::MSSA;
                using namespace std;
                // TODO: create test to add input_size to some array/vector and have all values be valid floating point values
                vector<float> test_arr = {};
                MSSAProcessingUnit<float>::CreateValidSegment(test_csv, test_arr);
                MSSAProcessingUnit<float>::SegmentInput(test_arr);
                EXPECT_FLOAT_EQ(test_arr[0], 4);
            }

            TEST_F(PreProcessorTest, CreateSegments) {

            }
        }  // namespace
    }  // namespace project
}  // namespace my