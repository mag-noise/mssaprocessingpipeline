#pragma once
#include <string>
#include <forward_list>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
#include <fstream>
#include <forward_list>
#include <iterator>
#include <vector>
#include <cstdarg>
namespace Processor{
    using namespace std;
    using Eigen::Dynamic;
    class MSSA{
        private:
#ifdef HEAPIFY
            const static bool is_dynamic = false;
#else
            const static bool is_dynamic = true;
#endif // HEAPIFY
            static int dynamic_input;
            static int dynamic_window;
            static int dynamic_k;
            const static int input_size = 5000;
            const static int window_size = 40;
            const static int k = input_size - window_size + 1;
        public:
            // TODO: Potentially move out values to definition utility file for central point of reference
            // --> Could also move class to be templated out, with typedefs defined outside and imported in.
            // --> if possible, it would make testing and production much faster to swap between while providing
            // --> extension opportunities
            const static int number_of_signals = 2;


            typedef std::conditional< is_dynamic, std::vector<double>, std::array<double, input_size>>::type ValidSignal;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, input_size, window_size* number_of_signals*number_of_signals>>::type ReconstructionMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, input_size, input_size>>::type CovMatrix;
        private:
            // TODO: Double: march 11, 2016 | 
            typedef std::conditional< is_dynamic, Eigen::Vector<double, Dynamic>, Eigen::Vector<double, window_size + k - 1>>::type SkewVector;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, window_size, k>>::type SignalMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, window_size* number_of_signals, k>>::type TrajectoryMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, window_size* number_of_signals, window_size* number_of_signals>>::type TrajCovarianceMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, window_size* number_of_signals, window_size* number_of_signals>>::type EigenVectorMatrix;
            typedef std::conditional< is_dynamic, Eigen::Matrix<double, Dynamic, Dynamic>, Eigen::Matrix<double, window_size* number_of_signals, k>>::type ProjectionMatrix;
            // Separated units of work for debugging purposes
            // TODO: fill out implemention step-wise
            TrajectoryMatrix static GenerateTrajectoryMatrix(ValidSignal &inboard_signal, ValidSignal &outboard_signal);
            ReconstructionMatrix static GenerateProjection(TrajectoryMatrix trajectory);
            SkewVector static SkewVectorAverage(SignalMatrix proj);
            ReconstructionMatrix static ReconstructMatrix(ProjectionMatrix proj, EigenVectorMatrix eig);

        public:
            void static DynamicVariableSetup(int, int);
            int static InputSize();
            int static WindowSize();
            int static RemainingValues();
            //CovMatrix static GenerateCovarianceMatrix(ValidSignal vectorA, ValidSignal vectorB);
            ReconstructionMatrix static Process(ValidSignal &inboard_signal, ValidSignal &outboard_signal);
            double static CorrelationCoefficient(Eigen::MatrixXd x, Eigen::MatrixXd y);
            std::vector<int> static ComponentSelection(ReconstructionMatrix recon, ValidSignal inboard, ValidSignal outboard, double alpha);
            

    };
}