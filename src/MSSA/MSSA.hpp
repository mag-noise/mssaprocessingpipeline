#pragma once
#include <string>
#include <Eigen/Dense>
namespace Processor{
    using namespace std;
    class MSSA{
        private:
            const static int input_size = 100;
            const static int window_size = 40;
            const static int k = input_size - window_size + 1;
            const static int number_of_signals = 2;
            typedef Eigen::Matrix<double, window_size, k> SignalMatrix;
            typedef Eigen::Matrix<double, window_size* number_of_signals, k> TrajectoryMatrix;
            typedef Eigen::Matrix<double, window_size* number_of_signals, window_size* number_of_signals> TrajCovarianceMatrix;
            typedef Eigen::Matrix<double, window_size* number_of_signals, window_size* number_of_signals> EigenVectorMatrix;
            typedef Eigen::Matrix<double, window_size* number_of_signals, k> ProjectionMatrix;
            typedef Eigen::Matrix<double, input_size, window_size* number_of_signals> ReconstructionMatrix;
            typedef Eigen::Vector<double, window_size* number_of_signals + k - 1> SkewVector;
        // Separated units of work for debugging purposes
        // TODO: fill out implemention step-wise
            TrajectoryMatrix static GenerateTrajectoryMatrix(array<double, input_size> &input_signal1, array<double, input_size> &input_signal2);
            ReconstructionMatrix static GenerateProjection(TrajectoryMatrix trajectory);
            SkewVector static SkewVectorAverage(ProjectionMatrix proj);
            ReconstructionMatrix static ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig);
    public:
        // Name: Process
        // Description: take input signal stream filter out noise from system
        // Input: multi-channel double array
        // Output:
        void static Process(array<double, input_size> input_signal1, array<double, input_size> input_signal2);
    };
}