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

            class SpuTest : public ::testing::Test {
            public:
                std::string test_csv;
                std::u16string test_matfile;
            protected:

                SpuTest() {
                    // TODO: change to relative reference
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                    test_matfile = u"C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\2016\\03\\11\\MGF\\MGF_20160311_064011_065832_v2.1.0.lv2";
                    Processor::MSSA::DynamicVariableSetup(/*Input Size*/30000, /*Window Size*/40);
                }

                ~SpuTest() override {
                }

            };


            TEST_F(SpuTest, CorrectlyReadsCSVFile) {
                using SignalProcessingUnit::MSSAProcessingUnit;
                using Processor::MSSA;
                using namespace std;
                // TODO: create test to add input_size to some array/vector and have all values be valid double precision values
                vector<double> test_arr = {};
                MSSAProcessingUnit<double> unit = MSSAProcessingUnit<double>(true);
                unit.LoadCSVData(test_csv, test_arr);
                EXPECT_DOUBLE_EQ(test_arr[0], 4);
            }

            TEST_F(SpuTest, ReadMatfile) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> unit = MSSAProcessingUnit<double>(true);
                unit.LoadFromMatlab(test_matfile);
                EXPECT_NE(unit['x'][0][0], 0.0);

            }

            TEST_F(SpuTest, OutputFormattedSignal) {
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double> mssaCpu = MSSAProcessingUnit<double>(true);
            }

            TEST_F(SpuTest, MatFileProcessing) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> in_unit = MSSAProcessingUnit<double, vector<double>>(true);
                MSSAProcessingUnit<double, vector<double>> in_unit_copy = MSSAProcessingUnit<double, vector<double>>(true);

                MSSAProcessingUnit<double, vector<double>> out_unit = MSSAProcessingUnit<double, vector<double>>(false);

                in_unit.LoadFromMatlab(test_matfile);
                in_unit_copy.LoadFromMatlab(test_matfile);
                out_unit.LoadFromMatlab(test_matfile);

                MSSAProcessingUnit<double, vector<double>>::Process(in_unit, out_unit);

                auto changed = in_unit.Join();
                auto original = in_unit_copy.Join();

                for (auto i = 0; i < in_unit.size(); i++) {
                    EXPECT_NEAR(original[i], changed[i], 0.1);

                }

            }

        }  // namespace
    }  // namespace CpuTesting
}  // namespace Testing