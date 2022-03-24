#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    // NOTE: Stack size limit 128 KB
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

    MSSA::ReconstructionMatrix MSSA::GenerateProjection(TrajectoryMatrix trajectory)
    {
        Eigen::SelfAdjointEigenSolver<TrajCovarianceMatrix> solver;
        solver.compute(trajectory * (trajectory.transpose()) / k);
        return ReconstructMatrix((solver.eigenvectors().transpose())*trajectory, solver.eigenvectors());
    }

    MSSA::SkewVector MSSA::SkewVectorAverage(ProjectionMatrix proj)
    {
        SkewVector builder;

        for (auto i = 0; i < window_size*2; i++) {
            for (auto j = 0; j < k; j++) {
                builder[i + j] += proj(i, j)/(i+j+1);
            }
        }
        return builder;
    }

    MSSA::ReconstructionMatrix MSSA::ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig)
    {

        return ReconstructionMatrix();
    }

    
    
}