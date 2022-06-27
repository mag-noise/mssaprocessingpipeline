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

    //Initialization of dynamic statics
    int MSSA::dynamic_input;
    int MSSA::dynamic_window;
    int MSSA::dynamic_k;

    // PRIVATE FUNCTIONS
#pragma region MSSA_PRIVATE_REGION

    MSSA::TrajectoryMatrix MSSA::GenerateTrajectoryMatrix(ValidSignal &inboard_signal, ValidSignal &outboard_signal) {
        TrajectoryMatrix t = TrajectoryMatrix(WindowSize() * number_of_signals, RemainingValues());
        for (auto i = 0; i < WindowSize(); i++) {
            for (auto j = 0; j < RemainingValues(); j++) {
                t(i, j) = inboard_signal[i + j];
                t(i+ WindowSize(), j) = outboard_signal[i + j];
            }
        }

        return t;
    }

    MSSA::ReconstructionMatrix MSSA::GenerateProjection(TrajectoryMatrix trajectory)
    {
        Eigen::EigenSolver<TrajCovarianceMatrix> solver;
        solver.compute(trajectory * (trajectory.transpose()) / RemainingValues());
        // Sends the projection matrix along with the solver's eigenvectors
#ifdef _TEST
        string path = "eigenvector.csv";
        ofstream ofs(path);

        // CSV Formatting example: https://stackoverflow.com/questions/61987600/write-eigen-vectorxd-in-csv-format
        using namespace Eigen;
        IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
        ofs << (solver.eigenvectors().real()).format(OctaveFmt);
        ofs.close();
#endif
        // Need to consider solver.info() == Eigen::NoConvergence vs Eigen::Success
        return ReconstructMatrix((solver.eigenvectors().real().transpose()) * trajectory, solver.eigenvectors().real());
    }

    MSSA::SkewVector MSSA::SkewVectorAverage(SignalMatrix proj)
    {
        SkewVector builder=SkewVector(WindowSize() + RemainingValues() - 1);
        SkewVector counter=SkewVector(WindowSize() + RemainingValues() - 1);

        builder.setZero();
        counter.setZero();
        for (auto i = 0; i < WindowSize(); i++) {
            for (auto j = 0; j < RemainingValues(); j++) {
                builder[i + j] += proj(i, j);
                counter[i + j] += 1;
            }
        }
        for (auto i = 0; i < WindowSize() + RemainingValues() - 1; i++)
            builder[i] = builder[i] / counter[i];

        return builder;
    }

    MSSA::ReconstructionMatrix MSSA::ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig)
    {
        ReconstructionMatrix rMatrix = ReconstructionMatrix(InputSize(), WindowSize() * number_of_signals * number_of_signals);
        /*Eigen::Vector<double, window_size> selectedEigVector;
        Eigen::Vector<double, k> projectionRowVector;
        Eigen::Matrix<double, window_size, k> endVector;*/
        Eigen::MatrixXd selectedEigVector;
        Eigen::MatrixXd projectionRowVector;
        Eigen::MatrixXd endVector;
        for (auto m = 0; m < WindowSize() * number_of_signals; m++) {
            for (auto sig_m = 0; sig_m < number_of_signals; sig_m++) {
                // Reconstruction matrix of 1 input = NxLM, thus matrix created to hold NxLM^2 elements to allow an NxLM matrix for each M.
                // Skew vector takes a matrix LxK constructed from an L vector crossed with K vector, 
                // then generates an N vector of averages from the LxK skew diagonals
                selectedEigVector = (eig.col(m)(Eigen::seq(sig_m * WindowSize(), (sig_m + 1) * WindowSize() - 1)));

                // Note: .row().traspose() does not play well with * operator. Likely need to store. 
                // Due to size, could be dynamically stored without massive computational speed loss. Worth exploring if needed
                projectionRowVector = proj.row(m);
                endVector = selectedEigVector *(projectionRowVector);
                rMatrix.col(m + WindowSize() * number_of_signals * sig_m) = SkewVectorAverage(endVector);
#ifdef _TEST
                if(m==0){
                    cout << "selectedEigenVector: " << selectedEigVector << endl;
                    cout << "projectionRowVector: " << projectionRowVector << endl;
                    cout << "endVector: " << endVector << endl;
                    cout << "rMatrix: " << rMatrix.col(m + WindowSize() * number_of_signals * sig_m) << endl;
                }
#endif


            }
            
        }
        return rMatrix;
    }




#pragma endregion


#pragma region MSSA_PUBLIC_REGION


    // PUBLIC FUNCTIONS
    void MSSA::DynamicVariableSetup(int input, int window) {
        if (MSSA::dynamic_input && MSSA::dynamic_window && MSSA::dynamic_k)
            return;
        MSSA::dynamic_input = input;
        MSSA::dynamic_window = window;
        MSSA::dynamic_k = input - window + 1;
    }

    //Functions to use static or dynamic values
    int MSSA::InputSize() {
        return !is_dynamic * input_size + is_dynamic * dynamic_input;
    }
    
    int MSSA::WindowSize() {
        return !is_dynamic * window_size + is_dynamic * dynamic_window;
    }
    
    int MSSA::RemainingValues() {
        return !is_dynamic * k + is_dynamic * dynamic_k;
    }

    // NOTE: Stack size limit 128 KB
    MSSA::ReconstructionMatrix MSSA::Process(ValidSignal&inboard_signal, ValidSignal &outboard_signal) {
        assert(
            std::size(inboard_signal) == InputSize() && std::size(outboard_signal) == InputSize()
        );
        return GenerateProjection(GenerateTrajectoryMatrix(inboard_signal, outboard_signal));
    }

    // TODO: Create Covariance Matrix for 2 vectors
    //MSSA::CovMatrix MSSA::GenerateCovarianceMatrix(ValidSignal vectorA, ValidSignal vectorB) {
    //    using Eigen::VectorXd;
    //    MatrixXd vecA = Eigen::Map<Eigen::Matrix<double, 1, MSSA::input_size>>(vectorA.data());
    //    MatrixXd vecB = Eigen::Map<Eigen::Matrix<double, 1, MSSA::input_size>>(vectorB.data());
    //    MatrixXd mat = MatrixXd(2, MSSA::input_size);
    //    mat.row(0) = vecA;
    //    mat.row(1) = vecB;

    //    MatrixXd centered = mat.rowwise() - mat.colwise().mean();
    //    MatrixXd cov = (centered.adjoint() * centered) / double(mat.rows());
    //    return cov;
    //}

    double MSSA::CorrelationCoefficient(Eigen::MatrixXd x, Eigen::MatrixXd y) {
        assert(x.size() == y.size() && x.size() == InputSize());
        
        double x_m = x.mean();
        double y_m = y.mean();
        x.array() -= x_m;
        y.array() -= y_m;
        double val2 = (sqrt((x.array() * x.array()).sum()) * sqrt((y.array() * y.array()).sum())); 
        if (std::isnan(val2))
            return 0;

        if(x.cols() != y.cols())
            return (x.transpose().array() * y.array()).sum() / val2;
        else
            return (x.array() * y.array()).sum() / val2;
    }

    /// <summary>
    /// Gets a list of indices that are required for reconstruction
    /// </summary>
    /// <param name="recon"></param>
    /// <param name="inboard"></param>
    /// <param name="outboard"></param>
    /// <returns></returns>
    std::forward_list<int> MSSA::ComponentSelection(ReconstructionMatrix recon, ValidSignal inboard, ValidSignal outboard, double alpha) {
        Eigen::Map<Eigen::MatrixXd> x(inboard.data(), 1, InputSize());
        Eigen::Map<Eigen::MatrixXd> y(outboard.data(), 1, InputSize());
        MatrixXd interference = x-y;

#ifdef _DEBUG
        auto xsize = x.size();
        auto ysize = y.size();

#endif

         //MatrixXd interference = Eigen::Map<Eigen::Matrix<double, 1, input_size>>(inboard.data()) - Eigen::Map<Eigen::Matrix<double, 1, input_size>>(outboard.data());
        //double alpha = 0.005;
        std::forward_list<int> indexList = std::forward_list<int>();
        for (int i = 0; i < recon.cols(); i++) {
            auto check = abs(CorrelationCoefficient(recon.col(i), interference));
            if (check > alpha) {
                indexList.push_front(i);
            }

        }
        return indexList;
    }


    
#pragma endregion MSSA_PUBLIC_REGION
    
}