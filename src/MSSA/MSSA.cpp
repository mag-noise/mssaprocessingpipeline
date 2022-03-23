#include "MSSA.hpp"
#include <Eigen/Dense>
#include <string>
#include <iostream>
namespace Processor{
    using namespace std;
    using Eigen::MatrixXd;

    void MSSA::Process(double input_signal1[], double input_signal2[]) {
        const int k = input_size - window_size + 1;
        MatrixXd m(window_size, k);
        MatrixXd n(window_size, k);
        
    }

    MatrixXd generateMatrix
    
}