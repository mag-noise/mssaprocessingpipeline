#include "../src/MSSA/MSSA.hpp"
#include "gtest/gtest.h"
#include <Eigen/Dense>
#include <cmath>
#include <numeric>
#include <string>

namespace Testing {
    namespace Processing {
        namespace {

            // The fixture for testing class Foo.
            class ProcessorTest : public ::testing::Test {};


            // Tests that Foo does Xyz.
            TEST_F(ProcessorTest, TestCovariance) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> vecA = { 5, 12, 18, 23, 45 };
                vector<double> vecB = { 2, 8, 18, 20, 28 };

                auto cov = Processor::MSSA::GenerateCovarianceMatrix(vecA, vecB);

                MatrixXd compMat(5,5);
                compMat << 9.0 / 4.0, 3.0, 0, 9.0 / 4.0, 51.0 / 4.0,
                    3, 4, 0, 3, 17,
                    0, 0, 0, 0, 0,
                    9.0 / 4.0, 3, 0, 9.0 / 4.0, 51.0 / 4.0,
                    51.0 / 4.0, 17, 0, 51.0 / 4.0, 289.0 / 4.0;
                cout << "compMat: " << endl << compMat << endl;

                cout << "cov: " << endl << cov.block<5, 5>(0, 0) << endl;

                EXPECT_EQ(compMat.isApprox(cov.block<5, 5>(0, 0)), true);
            }
            // Tests that Foo does Xyz.
            TEST_F(ProcessorTest, TestCovariance2) {
                // Exercises the Xyz feature of Foo.
                using namespace std;
                using Eigen::MatrixXd;

                vector<double> vecA = { 4, 12, 67, 23, 45 };
                vector<double> vecB = { 2, 8, 16, 20, 2 };

                auto cov = Processor::MSSA::GenerateCovarianceMatrix(vecA, vecB);

                MatrixXd compMat(5, 5);
                compMat << 1, 2, 51.0/2.0, 3.0/ 2.0, 43.0 / 2.0,
                    2, 4, 51, 3, 43,
                    51.0/2.0, 51, 2601.0/4.0, 153.0/4.0, 2193.0/4.0,
                    3.0 / 2.0, 3, 153.0/4, 9.0 / 4.0, 129.0 / 4.0,
                    43.0 / 2.0, 43, 2193.0/4.0, 129.0 / 4.0, 1849.0 / 4.0;
                cout << "compMat: " << endl << compMat << endl;

                cout << "cov: " << endl << cov.block<5,5>(0,0) << endl;

                EXPECT_EQ(compMat.isApprox(cov.block<5, 5>(0, 0)), true);
            }

        }  // namespace
    }  // namespace project
}  // namespace my