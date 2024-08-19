#include "../src/Utilities/ModelInjector.hpp"
#include "gtest/gtest.h"
#include <Eigen/dense>
#include "../src/MSSA/MSSA.hpp"

namespace Testing {
    namespace Injection {
        namespace {
            using namespace Utils;
            class InjectorTest : public ::testing::Test {
            
            public:
                Utils::Injector& testInjector = Utils::Injector::GetInstance();

            protected:

                InjectorTest() {
                    testInjector.LoadModel("C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\MSSAProcessingPipeline\\models\\FCN_20E_20240614_120751.pt");
                }

                ~InjectorTest() override {
                }

            };

        }
        TEST_F(InjectorTest, GenerateInjector) {
            using namespace Utils;
            auto testMatrix = Eigen::MatrixXd();
            std::vector<double>inboard(5000, 0);
            std::vector<double>outboard(5000, 1);
            testMatrix = Processor::MSSA::Process(inboard, outboard);

            testInjector.ApplyModel(testMatrix, inboard, outboard, 0.05);
            EXPECT_EQ(true, true);
        }

    }
}