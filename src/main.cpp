#include "MSSA/MSSA.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>

int main() {
    using namespace std;
    using Eigen::MatrixXd;
    std::array<double, 100> input1;
    std::array<double, 100> input2;
    for_each(input1.begin(), input1.end(), [](double& n) { n = (rand() % 5); });
    for_each(input2.begin(), input2.end(), [](double& n) { n = (rand() % 5); });
    
    
    string path = "log.csv";
    ofstream ofs(path);
    if (!ofs)
        return -1;

    // CSV Formatting example: https://stackoverflow.com/questions/61987600/write-eigen-vectorxd-in-csv-format
    const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ");
    ofs << Processor::MSSA::Process(input1, input2).format(CSVFormat);
    return 0;
}
