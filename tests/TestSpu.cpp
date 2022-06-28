#include "../src/MSSA/MSSA.hpp"
#include "../src/SPU/SPU.hpp"

#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

namespace Testing {
    namespace SpuTesting {
        namespace {

            class SpuTest : public ::testing::Test {
            
            public:
                std::string test_csv;
                std::u16string test_matfile;
                std::u16string data_file[3];

                std::u16string Matfile(int file_num) {

                    return test_matfile + data_file[file_num];
                }

            protected:

                SpuTest() {
                    // TODO: change to relative reference
                    test_csv = "C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\signal1.csv";
                    /*
                    Files:
                    1. MGF_20160311_064011_065832_v2.1.0.lv2
                    2. MGF_20160311_082211_084032_v2.1.0.lv2
                    3. MGF_20160311_234614_235232_v2.1.0.lv2
                    */

                    data_file[0] = u"MGF_20160311_064011_065832_v2.1.0.lv2";
                    data_file[1] = u"MGF_20160311_082211_084032_v2.1.0.lv2";
                    data_file[2] = u"MGF_20160311_234614_235232_v2.1.0.lv2";
                    test_matfile = u"C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\2016\\03\\11\\MGF\\";
                    Processor::MSSA::DynamicVariableSetup(/*Input Size*/25000, /*Window Size*/40);
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
#ifdef _MAT_

            TEST_F(SpuTest, ReadMatfile) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> unit = MSSAProcessingUnit<double>(true);
                unit.LoadFromMatlab(Matfile(0));
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

                in_unit.LoadFromMatlab(Matfile(0));
                in_unit_copy.LoadFromMatlab(Matfile(0));
                out_unit.LoadFromMatlab(Matfile(0));

                MSSAProcessingUnit<double, vector<double>>::Process(in_unit, out_unit);

                auto changed = in_unit.JoinSignal();
                auto original = in_unit_copy.JoinSignal();

                for (auto i = 0; i < in_unit.size(); i++) {
                    EXPECT_NEAR(original[i], changed[i], 0.1);

                }

            }

            TEST_F(SpuTest, MatNaNProcessing) {
                using namespace std;
                using namespace SignalProcessingUnit;
                MSSAProcessingUnit<double, vector<double>> in_unit = MSSAProcessingUnit<double, vector<double>>(true);


                in_unit.LoadFromMatlab(Matfile(1));


            }
#endif //_MAT_

        }  // namespace
    }  // namespace CpuTesting
}  // namespace Testing