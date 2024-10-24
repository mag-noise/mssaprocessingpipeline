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
namespace Processor {
    using namespace std;
    using Eigen::MatrixXd;

    /// <summary>
    /// Class for processing signals through MSSA. Provides public interfaces 
    /// </summary>
    class WAICUP {
    private:

        //"General Parameters"
        unsigned int ui_uf;            // Uniform Filter Size for detrending
        bool b_detrend;     // Detrend the data

        //"Algorithm Parameters"
        unsigned int ui_fs;              // Sampling Frequency
        float f_dj;           // Wavelet Scale Spacing
        float f_scales;       // Scales used in the wavelet transform
        float f_lowest_freq;  // Lowest frequency in the wavelet transform
        float f_boom;         // Trend to use during retrending process

    public:

        WAICUP(unsigned int uf = 400, bool detrend=true, unsigned int fs=1, float dj=1/12, float scales=0, float lowest_freq=0, float boom=0)
            : ui_uf(uf), b_detrend(detrend), ui_fs(fs), f_dj(dj), f_scales(scales), f_lowest_freq(lowest_freq), f_boom(boom)
        {}


        int Clean(MatrixXd B_field, bool triaxial);

        int CleanWAICUP(MatrixXd sensors);

        int Dual(MatrixXd signal, MatrixXd dt, MatrixXd dj);

        int Multi(MatrixXd signal, MatrixXd dt, MatrixXd dj)

    };
}