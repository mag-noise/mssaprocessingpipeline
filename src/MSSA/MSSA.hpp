#pragma once
#include <string>
#include <forward_list>
#include <Eigen/Dense>
#include <vector>
namespace Processor{
    using namespace std;
    using Eigen::Dynamic;
    class MSSA{
        private:
            const static bool is_dynamic = true;

        public:
            // TODO: Potentially move out values to definition utility file for central point of reference
            // --> Could also move class to be templated out, with typedefs defined outside and imported in.
            // --> if possible, it would make testing and production much faster to swap between while providing
            // --> extension opportunities
            const static int input_size = 100;
            const static int window_size = 40;
            const static int k = input_size - window_size + 1;
            const static int number_of_signals = 2;
            typedef std::conditional< is_dynamic, std::vector<float>, std::array<float, input_size>> CleanSignal;
        private:
            // TODO: Double: march 11, 2016 | 
            typedef std::conditional< is_dynamic, Eigen::Vector<float, Dynamic>, Eigen::Vector<float, window_size + k - 1>> SkewVector;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, window_size, k>> SignalMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, window_size* number_of_signals, k>> TrajectoryMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, window_size* number_of_signals, window_size* number_of_signals>> TrajCovarianceMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, window_size* number_of_signals, window_size* number_of_signals>> EigenVectorMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, window_size* number_of_signals, k>> ProjectionMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<float, Dynamic, Dynamic>, Eigen::Matrix<float, input_size, window_size* number_of_signals*number_of_signals>> ReconstructionMatrix;

            // Separated units of work for debugging purposes
            // TODO: fill out implemention step-wise
            TrajectoryMatrix static GenerateTrajectoryMatrix(array<float, input_size> &input_signal1, array<float, input_size> &input_signal2);
            ReconstructionMatrix static GenerateProjection(TrajectoryMatrix trajectory);
            SkewVector static SkewVectorAverage(Eigen::Matrix<float, window_size, k> proj);
            ReconstructionMatrix static ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig);

        public:
            // Name: Process
            // Description: take input signal stream filter out noise from system
            // Input: multi-channel double array
            // Output:
            MSSA::ReconstructionMatrix static Process(array<float, input_size> &input_signal1, array<float, input_size> &input_signal2);
            CleanSignal static BuildSignal(ReconstructionMatrix mat, forward_list<int> iarr_of_indices);

    };
}