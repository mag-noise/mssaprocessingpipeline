#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    // NOTE: Stack size on Windows: 1 MB - on Linux: 8 MB
    void MSSA::Process(array<double, input_size> input_signal1, array<double, input_size> input_signal2) {
        assert(
            sizeof(input_signal1) / sizeof(input_signal1[0]) == input_size  
            && sizeof(input_signal2) / sizeof(input_signal2[0]) == input_size
        );
        GenerateProjection(GenerateTrajectoryMatrix(input_signal1, input_signal2));
        
    }

    MSSA::TrajectoryMatrix MSSA::GenerateTrajectoryMatrix(array<double, input_size> &input_signal1, array<double, input_size> &input_signal2) {
        TrajectoryMatrix t;
        for (auto i = 0; i < window_size; i++) {
            for (auto j = 0; j < k; j++) {
                t(i, j) = input_signal1[i + j];
                t(i+window_size, j) = input_signal2[i + j];
            }
        }
        return t;
    }

    MSSA::ProjectionMatrix MSSA::GenerateProjection(TrajectoryMatrix trajectory)
    {
        TrajCovarianceMatrix c = trajectory*(trajectory.transpose())/k;
        Eigen::EigenSolver<TrajCovarianceMatrix> solver(c);
        return solver.eigenvectors().transpose()*trajectory;
    }
    
    
}