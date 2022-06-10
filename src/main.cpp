#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
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
    //using namespace std;
    //using namespace SignalProcessingUnit;
    //MSSAProcessingUnit<double, vector<double>> in_unit = MSSAProcessingUnit<double, vector<double>>(true);
    //MSSAProcessingUnit<double, vector<double>> out_unit = MSSAProcessingUnit<double, vector<double>>(false);
    //std::u16string test_matfile = u"C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\data\\2016\\03\\11\\MGF\\MGF_20160311_064011_065832_v2.1.0.lv2";
    //if (argc > 1)
    //    std::copy(argv[1], argv[1] + strlen(argv[1]) - 1, test_matfile);

    //in_unit.LoadFromMatlab(test_matfile);
    //out_unit.LoadFromMatlab(test_matfile);

    //MSSAProcessingUnit<double, vector<double>>::Process(in_unit, out_unit);

    return 0;
}
