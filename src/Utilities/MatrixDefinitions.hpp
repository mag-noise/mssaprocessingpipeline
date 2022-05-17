#pragma once
#include <string>
#include <forward_list>
#include <Eigen/Dense>
namespace Utils{
    

    template <typename input_type, size_t input_size, size_t window_size>
    class StaticMatrixDefinitions{
        typedef std::array<input_type, input_size> CleanSignal;
        typedef Eigen::Vector<input_type, window_size + k - 1> SkewVector;
        typedef Eigen::Matrix<input_type, window_size, k> SignalMatrix;
        typedef Eigen::Matrix<input_type, window_size* number_of_signals, k> TrajectoryMatrix;
        typedef Eigen::Matrix<input_type, window_size* number_of_signals, window_size* number_of_signals> TrajCovarianceMatrix;
        typedef Eigen::Matrix<input_type, window_size* number_of_signals, window_size* number_of_signals> EigenVectorMatrix;
        typedef Eigen::Matrix<input_type, window_size* number_of_signals, k> ProjectionMatrix;
        typedef Eigen::Matrix<input_type, input_size, window_size* number_of_signals* number_of_signals> ReconstructionMatrix;
    };

    template <typename input_type, size_t input_size, size_t window_size>
    class DynamicMatrixDefinitions{
        typedef std::vector<input_type> CleanSignal;
        typedef Eigen::Vector<input_type, Eigen::Dynamic> SkewVector;
        typedef Eigen::Matrix<input_type, Eigen::Dynamic, Eigen::Dynamic> SignalMatrix, TrajectoryMatrix, TrajCovarianceMatrix, EigenVectorMatrix , ProjectionMatrix, ReconstructionMatrix;
    };
}
