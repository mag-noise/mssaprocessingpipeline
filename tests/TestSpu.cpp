#include "../src/MSSA/MSSA.hpp"
#include "../src/SPU/SPU.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include <iostream>

namespace Testing {
    namespace SpuTesting {
        namespace {

            class DISABLED_SpuTest : public ::testing::Test {
            public:
                std::string test_csv;
                std::u16string test_matfile;
            protected:

                DISABLED_SpuTest() {
                    // TODO: change to relative reference
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                    test_matfile = u"C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\2016\\03\\11\\MGF\\MGF_20160311_064011_065832_v2.1.0.lv2";
                }

                ~DISABLED_SpuTest() override {
                }

            };


            TEST_F(DISABLED_SpuTest, CorrectlyReadsCSVFile) {
                using SignalProcessingUnit::MSSAProcessingUnit;
                using Processor::MSSA;
                using namespace std;
                // TODO: create test to add input_size to some array/vector and have all values be valid double precision values
                vector<double> test_arr = {};
                MSSAProcessingUnit<double> unit{};
                unit.LoadCSVData(test_csv, test_arr);
                EXPECT_DOUBLE_EQ(test_arr[0], 4);
            }

            TEST_F(DISABLED_SpuTest, ReadMatfile) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> unit{};
                unit.LoadFromMatlab(test_matfile, true);
                EXPECT_NE(unit['x'][0][0], 0.0);

            }

            TEST_F(DISABLED_SpuTest, OutputFormattedSignal) {
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double> mssaCpu = MSSAProcessingUnit<double>();
            }

            TEST_F(DISABLED_SpuTest, MatFileProcessing) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> in_unit{};
                MSSAProcessingUnit<double, vector<double>> out_unit{};

                in_unit.LoadFromMatlab(test_matfile, true);
                out_unit.LoadFromMatlab(test_matfile, false);

                MSSAProcessingUnit<double, vector<double>>::Process(in_unit, out_unit);

            }


        }  // namespace
    }  // namespace CpuTesting
}  // namespace Testing