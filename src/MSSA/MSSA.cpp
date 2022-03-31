#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
#include <fstream>
#include <forward_list>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXf;

    MSSA::TrajectoryMatrix MSSA::GenerateTrajectoryMatrix(array<float, input_size> &input_signal1, array<float, input_size> &input_signal2) {
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
        Eigen::EigenSolver<TrajCovarianceMatrix> solver;
        solver.compute(trajectory * (trajectory.transpose()) / k);
        // Sends the projection matrix along with the solver's eigenvectors

        string path = "eigenvector.csv";
        ofstream ofs(path);

        // CSV Formatting example: https://stackoverflow.com/questions/61987600/write-eigen-vectorxd-in-csv-format
        using namespace Eigen;
        IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
        ofs << (solver.eigenvectors().real()).format(OctaveFmt);

        return ReconstructMatrix((solver.eigenvectors().real().transpose()) * trajectory, solver.eigenvectors().real());
    }

    MSSA::SkewVector MSSA::SkewVectorAverage(Eigen::Matrix<float, window_size, k> proj)
    {
        SkewVector builder;
        builder.setZero();
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
        Eigen::Vector<float, window_size> selectedEigVector;
        Eigen::Vector<float, k> projectionRowVector;
        Eigen::Matrix<float, window_size, k> endVector;
        for (auto m = 0; m < window_size * number_of_signals; m++) {
            for (auto sig_m = 0; sig_m < number_of_signals; sig_m++) {
                // Reconstruction matrix of 1 input = NxLM, thus matrix created to hold NxLM^2 elements to allow an NxLM matrix for each M.
                // Skew vector takes a matrix LxK constructed from an L vector crossed with K vector, 
                // then generates an N vector of averages from the LxK skew diagonals
                selectedEigVector = (eig.col(m)(Eigen::seq(sig_m * window_size, (sig_m + 1) * window_size - 1)));

                // Note: .row().traspose() does not play well with * operator. Likely need to store. 
                // Due to size, could be dynamically stored without massive computational speed loss. Worth exploring if needed
                projectionRowVector = proj.row(m);
                endVector = selectedEigVector *(projectionRowVector.transpose());
                rMatrix.col(m + window_size * number_of_signals * sig_m) = SkewVectorAverage(endVector);
            }
            
        }
        return rMatrix;
    }

    // NOTE: Stack size limit 128 KB
    MSSA::ReconstructionMatrix MSSA::Process(array<float, input_size> &input_signal1, array<float, input_size> &input_signal2) {
        assert(
            sizeof(input_signal1) / sizeof(input_signal1[0]) == input_size  
            && sizeof(input_signal2) / sizeof(input_signal2[0]) == input_size
        );
        return GenerateProjection(GenerateTrajectoryMatrix(input_signal1, input_signal2));
    }

    MSSA::CleanSignal MSSA::BuildSignal(ReconstructionMatrix mat, std::forward_list<int> iarr_of_indices)
    {
        CleanSignal output_signal = {};

        for (const auto& x : iarr_of_indices) {
            for (auto val = 0; val < input_size; val++ ) {
                output_signal[val] += mat.col(x)[val];
            }
        }

        return output_signal;
    }

    
    
}