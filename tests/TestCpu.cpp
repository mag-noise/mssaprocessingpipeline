#include "../src/MSSA/MSSA.hpp"
#include "../src/CPU/CPU.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include <iostream>

namespace Testing {
    namespace CpuTesting {
        namespace {

            class CpuTest : public ::testing::Test {
            public:
                std::string test_csv;
                std::u16string test_matfile;
            protected:

                CpuTest() {
                    // TODO: change to relative reference
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                    test_matfile = u"C:\\Users\\klsteele\\Downloads\\FolderOfDoom\\2016-01-28_Equator_Crossing_16Hz_2RPM.mat";
                }

                ~CpuTest() override {
                }

            };


            TEST_F(CpuTest, CorrectlyReadsCSVFile) {
                using CentralProcessingUnit::MSSAProcessingUnit;
                using Processor::MSSA;
                using namespace std;
                // TODO: create test to add input_size to some array/vector and have all values be valid double precision values
                vector<double> test_arr = {};
                MSSAProcessingUnit<double>::LoadCSVData(test_csv, test_arr);
                EXPECT_DOUBLE_EQ(test_arr[0], 4);
            }

            TEST_F(CpuTest, OutputFormattedSignal) {
                using namespace CentralProcessingUnit;
                MSSAProcessingUnit<double> mssaCpu = MSSAProcessingUnit<double>();
            }

            TEST_F(CpuTest, ReadMatfile) {
                using namespace std;
                using namespace CentralProcessingUnit;
                vector<double> test_in = {};
                vector<double> test_out = {};
                MSSAProcessingUnit<double>::LoadFromMatlab(test_matfile, test_in, test_out);
                EXPECT_NE(test_in[0], 0);

            }
        }  // namespace
    }  // namespace CpuTesting
}  // namespace Testing