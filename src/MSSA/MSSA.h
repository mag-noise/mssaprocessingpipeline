#pragma once

namespace Processing{
    class MSSA{
        public:
            // Name: Process
            // Description: take input signal stream filter out noise from system
            // Input: multi-channel double array
            // Output:
            void Process();
        private:
            const int input_size = 200;
            const int window_size = 60;
        // Separated units of work for debugging purposes
        // TODO: fill out implemention step-wise

            void BuildTrajectory();
            void MatrixDecomposition();
            void Projection();
            void SkewAverage();
            void Reconstruct();
    };
}