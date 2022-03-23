#pragma once
#include <string>
namespace Processor{
    using namespace std;
    class MSSA{
        public:
            // Name: Process
            // Description: take input signal stream filter out noise from system
            // Input: multi-channel double array
            // Output:
            void static Process(double input_signal1[], double input_signal2[]);
        private:
            const static int input_size = 200;
            const static int window_size = 60;
        // Separated units of work for debugging purposes
        // TODO: fill out implemention step-wise

            void BuildTrajectory();
            void MatrixDecomposition();
            void Projection();
            void SkewAverage();
            void Reconstruct();
    };
}