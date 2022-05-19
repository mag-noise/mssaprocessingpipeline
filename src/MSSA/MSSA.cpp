#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
#include <fstream>
#include <forward_list>
#include <iterator>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    // PRIVATE FUNCTIONS

#pragma region MSSA_PRIVATE_REGION

    MSSA::TrajectoryMatrix MSSA::GenerateTrajectoryMatrix(ValidSignal &inboard_signal, ValidSignal &outboard_signal) {
        TrajectoryMatrix t = TrajectoryMatrix(window_size * number_of_signals, k);
        for (auto i = 0; i < window_size; i++) {
            for (auto j = 0; j < k; j++) {
                t(i, j) = inboard_signal[i + j];
                t(i+window_size, j) = outboard_signal[i + j];
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

    MSSA::SkewVector MSSA::SkewVectorAverage(SignalMatrix proj)
    {
        SkewVector builder=SkewVector(window_size + k - 1);
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
        ReconstructionMatrix rMatrix = ReconstructionMatrix(input_size, window_size * number_of_signals * number_of_signals);
        /*Eigen::Vector<double, window_size> selectedEigVector;
        Eigen::Vector<double, k> projectionRowVector;
        Eigen::Matrix<double, window_size, k> endVector;*/
        Eigen::MatrixXd selectedEigVector;
        Eigen::MatrixXd projectionRowVector;
        Eigen::MatrixXd endVector;
        for (auto m = 0; m < window_size * number_of_signals; m++) {
            for (auto sig_m = 0; sig_m < number_of_signals; sig_m++) {
                // Reconstruction matrix of 1 input = NxLM, thus matrix created to hold NxLM^2 elements to allow an NxLM matrix for each M.
                // Skew vector takes a matrix LxK constructed from an L vector crossed with K vector, 
                // then generates an N vector of averages from the LxK skew diagonals
                selectedEigVector = (eig.col(m)(Eigen::seq(sig_m * window_size, (sig_m + 1) * window_size - 1)));

                // Note: .row().traspose() does not play well with * operator. Likely need to store. 
                // Due to size, could be dynamically stored without massive computational speed loss. Worth exploring if needed
                projectionRowVector = proj.row(m);
                endVector = selectedEigVector *(projectionRowVector);
                rMatrix.col(m + window_size * number_of_signals * sig_m) = SkewVectorAverage(endVector);
            }
            
        }
        return rMatrix;
    }

    // TODO: Create Covariance Matrix for 2 vectors
    MSSA::CovMatrix MSSA::GenerateCovarianceMatrix(ValidSignal vectorA, ValidSignal vectorB) {
        using Eigen::VectorXd;
        MatrixXd vecA = Eigen::Map<Eigen::Matrix<double, 1, MSSA::input_size>>(vectorA.data());
        MatrixXd vecB = Eigen::Map<Eigen::Matrix<double, 1, MSSA::input_size>>(vectorB.data());
        MatrixXd mat = MatrixXd(2, MSSA::input_size);
        mat.row(0) = vecA;
        mat.row(1) = vecB;

        
        //mat.row(0) = vectorA.data();
        //mat.row(1) = vectorB.data();
        MatrixXd centered = mat.rowwise() - mat.colwise().mean();
        MatrixXd cov = (centered.adjoint() * centered) / double(mat.rows());
        return cov;
    }


#pragma endregion


#pragma region MSSA_PUBLIC_REGION


    // PUBLIC FUNCTIONS

    // NOTE: Stack size limit 128 KB
    MSSA::ReconstructionMatrix MSSA::Process(ValidSignal&inboard_signal, ValidSignal &outboard_signal) {
        assert(
            std::size(inboard_signal) == input_size
            && std::size(outboard_signal) == input_size
        );
        return GenerateProjection(GenerateTrajectoryMatrix(inboard_signal, outboard_signal));
    }

    MSSA::ValidSignal MSSA::BuildSignal(ReconstructionMatrix mat, std::forward_list<int> iarr_of_indices)
    {
        ValidSignal output_signal = {};

        for (const auto& x : iarr_of_indices) {
            for (auto val = 0; val < input_size; val++ ) {
                output_signal[val] += mat.col(x)[val];
            }
        }

        return output_signal;
    }

    
#pragma endregion MSSA_PUBLIC_REGION
    
}