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

            TEST_F(ProcessorTest, SignalOutputSimilar) {
                using namespace std;
                using Eigen::MatrixXd;
                using Processor::MSSA;
                std::array<double, 100> input1{ 
                    5.92675767,  5.33255382,  3.2079705 ,  2.34158143,  4.17617803,
                    3.43741286,  2.87449564,  2.21520318,  3.32622834,  3.42996689,
                    5.20410601,  3.95102886,  0.79697378,  4.50524364,  4.01725594,
                    3.30281604,  3.43981957,  3.3708352 ,  3.79366242,  3.66488521,
                    1.29396853,  2.72232083,  4.58909721,  4.44607144,  4.56620612,
                    3.98935529,  3.71999961,  2.34092837,  4.17526196,  3.21124498,
                    2.82361003,  4.44457208,  4.70565288,  3.48218755,  4.706225  ,
                    3.40846927,  3.08254238,  3.24154552,  4.72675515,  1.2700013 ,
                    3.44937623,  1.32239362,  4.33790295,  3.17530577,  3.60373767,
                    4.02413657,  3.49376509,  1.72292731,  1.78223542,  3.15281778,
                    1.19689214,  2.59011259,  2.21084827,  2.40330207,  2.10440316,
                    3.4518415 ,  0.76403223,  3.240197  ,  1.41866086,  2.31165928,
                    2.92742598,  2.93815431,  2.39671053,  0.83655472,  4.11887815,
                    2.71176297,  2.02553503,  1.44304728,  2.33082546,  1.33897835,
                    3.25356201,  0.56094856,  1.0520416 ,  1.33953385,  3.87663321,
                    1.44659443,  0.95228515, -0.19259297,  2.46937343,  1.38648044,
                    1.46707948,  1.05272144,  0.49276388,  1.14700618,  3.20423349,
                    2.69618268,  0.96239641,  1.00089799,  1.60841647,  0.74308656,
                    1.68989365,  1.87723848, -0.31600192,  1.5582401 ,  3.17549086,
                    1.4432804 ,  2.52337001,  1.04247026,  0.74742949, 3.26119408 };
                std::array<double, 100> input2{ 
                    3.68980594, 3.81523477, 3.66258614, 4.31130612, 4.06347277,
                    3.05483698, 3.74107836, 3.20381289, 3.16074761, 5.0617104 ,
                    2.70484884, 4.00043702, 3.48663288, 2.4720324 , 2.71601707,
                    2.62654914, 4.42814516, 2.78874622, 4.00549015, 4.64455641,
                    3.56358934, 3.78578877, 2.91526966, 2.92929559, 2.96190711,
                    4.04417636, 3.97032574, 2.70988383, 3.29815992, 3.40098899,
                    3.72524229, 3.06191791, 2.96938479, 3.10422277, 3.25639683,
                    3.19712644, 2.23651719, 2.83639356, 3.07668111, 3.57213349,
                    4.72907949, 2.5069914 , 2.46883427, 2.91767561, 3.0066529 ,
                    1.72493756, 2.17111338, 2.77573495, 3.75511603, 2.23616425,
                    2.84261554, 1.84059559, 2.15043773, 2.12532531, 2.74553003,
                    2.05659   , 2.39469667, 3.95178129, 1.71352164, 2.50867621,
                    2.58238477, 2.89486393, 2.13514846, 2.60036724, 1.53382599,
                    1.37121342, 2.85765112, 2.25764091, 1.91636778, 3.3518255 ,
                    1.60156591, 1.04521055, 2.24354966, 2.01311145, 1.7915001 ,
                    1.53782142, 2.01423727, 0.98747412, 2.94507799, 1.01866758,
                    2.49218815, 0.26122476, 1.38900646, 2.25385178, 1.53825793,
                    1.95659147, 2.2523691 , 0.96245193, 2.57573091, 1.26880578,
                    2.53701973, 2.22311208, 2.08306924, 0.79615746, 0.86959657,
                    3.12809112, 2.26572798, 1.83149428, 1.62185506, 1.59993581 };


                std::array<double, 100> comparisonSignal_rows012{ 
                    4.40828   , 3.87612862, 4.34906618, 3.71505304, 4.24935576,
                    3.7696739 , 4.10442873, 3.83066064, 3.97413289, 3.90793778,
                    3.82976011, 3.99497981, 3.641109  , 4.05109039, 3.51856038,
                    4.0662109 , 3.40856987, 4.07404065, 3.30093812, 4.05431982,
                    3.23664033, 3.96551115, 3.24541447, 3.84088989, 3.27827171,
                    3.66028754, 3.36063722, 3.43124574, 3.46117662, 3.2079212 ,
                    3.54014284, 3.00032804, 3.61585774, 2.79700077, 3.64606785,
                    2.64481757, 3.61644415, 2.55136528, 3.533751  , 2.50801564,
                    3.40074852, 2.54208542, 3.21046015, 2.64543076, 2.96036063,
                    2.7769086 , 2.69470922, 2.91894307, 2.41740433, 3.0525141 ,
                    2.17186966, 3.1325608 , 1.9979439 , 3.15236782, 1.87521451,
                    3.11358255, 1.83273732, 2.97793602, 1.89054993, 2.76471826,
                    2.00349842, 2.52142442, 2.15680234, 2.23845027, 2.34098758,
                    1.96414445, 2.4960041 , 1.71500833, 2.61983779, 1.50642915,
                    2.69260141, 1.36237731, 2.67596835, 1.30858446, 2.59406976,
                    1.33089676, 2.42718484, 1.41690555, 2.20669912, 1.56708726,
                    1.95123512, 1.72287124, 1.70333261, 1.85253979, 1.49732377,
                    1.9683189 , 1.30537071, 2.03233789, 1.18941501, 2.0469082 ,
                    1.11338809, 2.02897611, 1.0624902 , 1.93294865, 1.13997152,
                    1.77485349, 1.21914987, 1.62349759, 1.26106298, 1.54754539 };
                
                MatrixXd reconstruction = MSSA::Process(input1, input2);
                MatrixXd signal1 =
                    reconstruction.block(0, 0, MSSA::input_size, MSSA::window_size * MSSA::number_of_signals);

                // TODO Extend to use first 3 rows

                int base_sum = accumulate(input1.begin(), input1.end(), 0);
                int recon_sum = accumulate(comparisonSignal_rows012.begin(), comparisonSignal_rows012.end(), 0);
                EXPECT_EQ(base_sum, recon_sum);
            }

        }  // namespace
    }  // namespace project
}  // namespace my

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
