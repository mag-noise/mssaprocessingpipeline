#include "MSSA.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>
#ifdef DEBUG
#include <assert.h>
#endif // !

namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    //Initialization of dynamic statics
    int MSSA::dynamic_input;
    int MSSA::dynamic_window;
    int MSSA::dynamic_k;

    // PRIVATE FUNCTIONS
#pragma region MSSA_PRIVATE_REGION

    /// <summary>
    /// Builds the trajectory matrix based in the input signals
    /// </summary>
    /// <param name="inboard_signal"></param>
    /// <param name="outboard_signal"></param>
    /// <returns></returns>
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

    /// <summary>
    /// Uses the Trajectory Matrix to construct the eigenvectors. If eigenvector generation is successful, sends projection matrix to generate reconstruction matrix.
    /// </summary>
    /// <param name="trajectory"></param>
    /// <returns></returns>
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
        if (solver.info() != Eigen::Success)
            throw std::invalid_argument("Eigenvectors could not be computed. Error with input values");

        // Need to consider solver.info() == Eigen::NoConvergence vs Eigen::Success
        return ReconstructMatrix((solver.eigenvectors().real().transpose()) * trajectory, solver.eigenvectors().real());
    }

    /// <summary>
    /// Builds the skew vector average from some signal matrix LxK, where L = Window Size and K = Segment Size - L
    /// </summary>
    /// <param name="proj"></param>
    /// <returns></returns>
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

    /// <summary>
    /// Function to create the Reconstruction Matrix of NxLM, where N = segment size, L = Window Size, and M = Number of signals^2.
    /// </summary>
    /// <param name="proj"></param>
    /// <param name="eig"></param>
    /// <returns></returns>
    MSSA::ReconstructionMatrix MSSA::ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig)
    {
        ReconstructionMatrix rMatrix = ReconstructionMatrix(InputSize(), WindowSize() * number_of_signals * number_of_signals);
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
//#ifdef _TEST
//                if(m==0){
//                    cout << "selectedEigenVector: " << selectedEigVector << endl;
//                    cout << "projectionRowVector: " << projectionRowVector << endl;
//                    cout << "endVector: " << endVector << endl;
//                    cout << "rMatrix: " << rMatrix.col(m + WindowSize() * number_of_signals * sig_m) << endl;
//                }
//#endif


            }
            
        }
        return rMatrix;
    }




#pragma endregion


#pragma region MSSA_PUBLIC_REGION

    /// <summary>
    /// Setup instance of MSSA to use Dynamic variables
    /// </summary>
    /// <param name="input"></param>
    /// <param name="window"></param>
    void MSSA::DynamicVariableSetup(int input, int window) {
        assert(input > 0 && window > 0 && window < input);
        if (MSSA::dynamic_input && MSSA::dynamic_window && MSSA::dynamic_k)
            return;
        MSSA::dynamic_input = input;
        MSSA::dynamic_window = window;
        MSSA::dynamic_k = input - window + 1;
    }

    //Functions to use static or dynamic values

    /// <summary>
    /// Public interface for input size decision making
    /// </summary>
    /// <returns></returns>
    int MSSA::InputSize() {
        return (!is_dynamic || dynamic_input<=0) * input_size + is_dynamic * dynamic_input;
    }

    /// <summary>
    /// Public interface for window size decision making
    /// </summary>
    /// <returns></returns>
    int MSSA::WindowSize() {
        return (!is_dynamic || dynamic_window <= 0) * window_size + is_dynamic * dynamic_window;
    }
    
    /// <summary>
    /// Public interface for calculated remaining segment size decision making
    /// </summary>
    /// <returns></returns>
    int MSSA::RemainingValues() {
        return (!is_dynamic || dynamic_k <= 0) * k + is_dynamic * dynamic_k;
    }


    /// <summary>
    /// Public interface to process 2 signals through MSSA. NOTE: Stack size limit 128 KB
    /// </summary>
    /// <param name="inboard_signal"></param>
    /// <param name="outboard_signal"></param>
    /// <returns></returns>
    MSSA::ReconstructionMatrix MSSA::Process(ValidSignal&inboard_signal, ValidSignal &outboard_signal) {
        assert(
            std::size(inboard_signal) == InputSize() && std::size(outboard_signal) == InputSize()
        );
        try
        { 
        return GenerateProjection(GenerateTrajectoryMatrix(inboard_signal, outboard_signal));
        }
        catch (std::exception const& ex) {
            throw;
        }
    }

    /// <summary>
    /// Function calculations correlation coefficient between 2 vectors
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    double MSSA::CorrelationCoefficient(Eigen::MatrixXd x, Eigen::MatrixXd y) {
        assert(x.size() == y.size() && x.size() == InputSize());
        assert(
            (x.cols() == 1 || x.rows() == 1) && (y.cols() == 1 || y.rows() == 1)
        );
        
        double x_m = x.mean();
        double y_m = y.mean();
        x.array() -= x_m;
        y.array() -= y_m;
        double val2 = sqrt(((x.array() * x.array()).sum()) * ((y.array() * y.array()).sum()));
        if (std::isnan(val2) || val2 == 0)
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
    std::vector<int> MSSA::ComponentSelection(ReconstructionMatrix recon, ValidSignal inboard, ValidSignal outboard, double alpha) {
        Eigen::Map<Eigen::MatrixXd> x(inboard.data(), 1, InputSize());
        Eigen::Map<Eigen::MatrixXd> y(outboard.data(), 1, InputSize());
        MatrixXd interference = x-y;

        std::vector<int> indexList = std::vector<int>();
        std::vector<double> listOfCheckValues = std::vector<double>();
        //std::vector<double> listOfCheckValuesX = std::vector<double>();
        //std::vector<double> listOfCheckValuesY = std::vector<double>();

        for (int i = 0; i < recon.cols(); i++) {
            try {
                auto check = CorrelationCoefficient(recon.col(i), interference);
                auto checkX = CorrelationCoefficient(recon.col(i), x);
                auto checkY = CorrelationCoefficient(recon.col(i), y);
                if ((abs(check) > alpha && abs(check) > abs(checkX) && abs(check) > abs(checkY))||
                    (abs(abs(check) - abs(checkX)) < alpha) || abs(abs(check) - abs(checkY)) < alpha) {
                    indexList.push_back(i);
                }
                listOfCheckValues.push_back(check);
                //listOfCheckValuesX.push_back(checkX);
                //listOfCheckValuesY.push_back(checkY);
            }
            catch (std::exception const& ex) {
                std::string error_message = "";
                error_message.append(ex.what());
                error_message.append("\nError occurred in correlation coefficient calculations.");
                throw std::invalid_argument(error_message.c_str());

            }

        }
        //indexList = { 0, 1, 40, 41 };
        return indexList;
    }


    
#pragma endregion MSSA_PUBLIC_REGION
    
}