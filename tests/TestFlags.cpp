#include "../src/Utilities/MatrixDefinitions.hpp"
#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>

namespace Testing {
    namespace Flagging {
        namespace {

            // The fixture for testing class Foo.
            class FlagTest : public ::testing::Test {
            public:
                int size;
                int segment;
                Utils::FlagSystem* flags;
                

            protected:

                FlagTest() {
                    size = 200;
                    segment = 10;
                    flags = Utils::FlagSystem::GetInstance();
                    flags->Resize(size);
                }

                ~FlagTest() override {
                }

                void SetUp() override {
                    flags->Reset();
                }
            };

            // Tests that Foo does Xyz.
            TEST_F(FlagTest, TestZeroPathFlags) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<int> idx = vector<int>();
                flags->FindFlagInSegment(0, segment, idx);

                EXPECT_EQ(idx.size(), 20);

            }

            TEST_F(FlagTest, TestNanFlags) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> simple(size, 0.0);
                vector<int> idx = vector<int>();
                simple[25] = nan("-ind");
                flags->FlagNaN(simple);
                flags->FindFlagInSegment(0, segment, idx);

                EXPECT_EQ(idx.size(), 21);
                
                int vals[] = {15,16,17,18,19,190,191,192,193,194,195};
                for (auto i : vals) {
                    EXPECT_EQ((*flags)[i].merge_required, 1);
                }

            }

            TEST_F(FlagTest, TestMultiNanFlags) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> simple(size, 0.0);
                vector<int> idx = vector<int>();
                for (auto i = 25; i < 30; i++)
                    simple[i] = nan("-ind");
                
                simple[33] = nan("-ind");
                flags->FlagNaN(simple);
                flags->FindFlagInSegment(0, segment, idx);

                EXPECT_EQ(idx.size(), 20);

                int vals[] = { 15,16,17,18,19,190,191,192,193};
                for (auto i : vals) {
                    EXPECT_EQ((*flags)[i].merge_required, 1);
                }

                int drops[] = { 25,26,27,28,29,30,31,32,33 };
                for (auto i : drops)
                    EXPECT_EQ((*flags)[i].skipped_value, 1);

            }

            // Tests that Foo does Xyz.
            TEST_F(FlagTest, TestDiscontinuity) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> time = vector<double>(size);
                for (int i = 0; i < size; i++) {
                    time[i] = i + 10*(i>= size /2);
                }
                flags->FlagDiscontinuity(time);

                vector<int> idx = vector<int>();
                flags->FindFlagInSegment(0, segment, idx);

                EXPECT_EQ(idx.size(), 20);

            }

            // Tests that Foo does Xyz.
            TEST_F(FlagTest, TestNanWithMerges) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> simple(size, 0.0);
                vector<int> idx = vector<int>();
                simple[25] = nan("-ind");
                flags->FlagNaN(simple);
                flags->FindFlagInSegment(0, segment, idx, false, 50);

                EXPECT_EQ(idx.size(), 38);

                int vals[] = { 15,16,17,18,19,190,191,192,193,194 };
                for (auto i : vals) {
                    EXPECT_EQ((*flags)[i].merge_required, 1);
                }

                int novals[] = { 20,21,23,24,196, 197, 198, 199 };
                for (auto i : novals) {
                    EXPECT_EQ((*flags)[i].skipped_value, 1);
                }
            }

        }  // namespace
    }  // namespace project
}  // namespace my