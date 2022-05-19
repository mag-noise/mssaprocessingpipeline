#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main() {
    using namespace std;
    using Eigen::MatrixXd;
    using namespace Processor;
    using SignalProcessingUnit::MSSAProcessingUnit;

    //MSSAProcessingUnit<double, vector<double>> spu1{};

    //string path = "log.csv";
    //ofstream ofs(path);
    //if (!ofs)
    //    return -1;

    //// CSV Formatting example: https://stackoverflow.com/questions/61987600/write-eigen-vectorxd-in-csv-format
    //const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ");
    //Eigen::MatrixXd reconstruction = MSSA::Process(input1, input2);
    //ofs << reconstruction.format(CSVFormat);

    //MSSA::ValidSignal recon_signal = MSSA::BuildSignal(reconstruction, { 0, 1, 2 });

    //cout << "[";
    //for_each(recon_signal.begin(), recon_signal.end(), [](double& n) {cout << n << ", "; });
    //cout << "]" << endl;

    return 0;
}
