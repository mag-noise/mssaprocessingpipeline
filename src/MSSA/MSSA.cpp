#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    // NOTE: Stack size limit 128 KB
    MSSA::ReconstructionMatrix MSSA::Process(array<double, input_size> input_signal1, array<double, input_size> input_signal2) {
        assert(
            sizeof(input_signal1) / sizeof(input_signal1[0]) == input_size  
            && sizeof(input_signal2) / sizeof(input_signal2[0]) == input_size
        );
        return GenerateProjection(GenerateTrajectoryMatrix(input_signal1, input_signal2));
        
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

        // Sends the projection matrix along with the solver's eigenvectors
        return ReconstructMatrix((solver.eigenvectors().transpose())*trajectory, solver.eigenvectors());
    }

    MSSA::SkewVector MSSA::SkewVectorAverage(Eigen::Matrix<double, window_size, k> proj)
    {
        SkewVector builder;

        for (auto i = 0; i < window_size; i++) {
            for (auto j = 0; j < k; j++) {
                builder[i + j] += proj(i, j)/(i+j+1);
            }
        }
        return builder;
    }

    MSSA::ReconstructionMatrix MSSA::ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig)
    {
        ReconstructionMatrix rMatrix;
        Eigen::VectorXd testVector;
        Eigen::VectorXd testVector2;
        Eigen::MatrixXd endVector;
        for (auto m = 0; m < window_size * number_of_signals; m++) {
            for (auto sig_m = 0; sig_m < number_of_signals; sig_m++) {
                // Reconstruction matrix of 1 input = NxLM, thus matrix created to hold NxLM^2 elements to allow an NxLM matrix for each M.
                // Skew vector takes a matrix LxK constructed from an L vector crossed with K vector, 
                // then generates an N vector of averages from the LxK skew diagonals
                testVector = eig.col(m)(Eigen::seq(sig_m * window_size, (sig_m + 1) * window_size - 1));

                // Note: .row().traspose() does not play well with * operator. Likely need to store. 
                // Due to size, could be dynamically stored without massive computational speed loss
                testVector2 = proj.row(m);
                endVector = testVector*(testVector2.transpose());
                rMatrix.col(m + window_size * number_of_signals * sig_m) =
                    SkewVectorAverage(endVector);
                /*rMatrix.col(m + window_size*number_of_signals*sig_m) = 
                    SkewVectorAverage(eig.col(m)(Eigen::seq(sig_m*window_size, (sig_m+1)*window_size - 1))*(proj.row(m).transpose()));*/
            }
            
        }
        return rMatrix;
    }

    
    
}