#include "../src/MSSA/MSSA.hpp"
#include "../src/CPU/CPU.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>

namespace Testing {
    namespace CpuTesting {
        namespace {

            class CpuTest : public ::testing::Test {
            public:
                std::string test_csv;
            protected:

                // You can remove any or all of the following functions if their bodies would
                // be empty.

                CpuTest() {
                    // You can do set-up work for each test here.
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                }

                ~CpuTest() override {
                    // You can do clean-up work that doesn't throw exceptions here.
                }

            };


            TEST_F(CpuTest, CorrectlyReadsCSVFile) {
                using CentralProcessingUnit::MSSAProcessingUnit;
                using Processor::MSSA;
                using namespace std;
                // TODO: create test to add input_size to some array/vector and have all values be valid floating point values
                vector<float> test_arr = {};
                MSSAProcessingUnit<float>::LoadCSVData(test_csv, test_arr);
                EXPECT_FLOAT_EQ(test_arr[0], 4);
            }

            TEST_F(CpuTest, OutputFormattedSignal) {

            }
        }  // namespace
    }  // namespace project
}  // namespace my