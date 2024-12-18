/*
Author: Kevin Steele, Alex Hoffmann
Last Update : 10 / 24 / 2024

General Parameters
----------
uf : window size for uniform filter used to detrend the data
detrend : boolean for whether to detrend the data

Algorithm Parameters
----------
fs : sampling frequency
dj : wavelet scale spacing
scales : scales used in the wavelet transform(set by the algorithm)
*/


/*
    import numpy as np
    from wavelets import WaveletAnalysis
    from scipy.ndimage import uniform_filter1d
    import itertools
*/



#include "WAICUP.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <algorithm> // for transform
#include <functional> // for plus
#include <complex> // Potential value for time
#ifdef DEBUG
#include <assert.h>
#endif // !

namespace Processor {
    using namespace std;
    using Eigen::MatrixXd;

    /*
        B : magnetic field measurements from the sensor array(n_sensors, axes, n_samples)
        -> in Eigen's case, this means Vector = n_sensors, Row = axes, and Column = n_samples
        triaxial : boolean for whether to use triaxial or uniaxial ICA
    */
    MatrixXd WAICUP::Clean(std::vector<MatrixXd> B_field, bool triaxial) {
        
        if (triaxial) {
            auto result = MatrixXd::Zero(3, B_field[0].cols());
            for (int axis = 0; axis < 3; axis++ ) {
                result[axis] = this.CleanWAICUP(B_field[:, axis, : ]);

            }
            return result;
        }
        else {
            // "B: (n_sensors, n_samples)"
            auto result = this.CleanWAICUP(B_field);
            return result;
        }

    }

    Eigen::VectorXd WAICUP::CleanWAICUP(MatrixXd sensors) {
        float dt = 1 / this.ui_fs;
        Eigen::VectorXd result;
        MatrixXd trend;
        // "Detrend"
        if (this.b_detrend) {
            trend = uniform_filter1d(sensors, size = this.ui_uf);
            sensors = sensors - trend;
        }

        if (sensors.rows() == 2) 
        {
            result = this.Dual(sensors, dt, this.f_dj);
        }
        else 
        {
            result = this.Multi(sensors, dt, this.f_dj);
        }

        //"Retrend"
        if (this.b_detrend) {
            if (this.f_boom > -1) 
            {
                result += trend[this.f_boom];
            }
            else 
            {
                // TODO: Average function
                result += np.mean(trend, axis = 0);
            }
        }

        return result;

    }

    Eigen::VectorXd WAICUP::Dual(MatrixXd signal, float dt, float dj) {

        //"Create Wavelets"
        WaveletAnalysis w1 = new WaveletAnalysis(sig[0], dt = dt, frequency = true, dj = dj, unbias = false, mask_coi = true);
        WaveletAnalysis w2 = new WaveletAnalysis(sig[1], dt = dt, frequency = true, dj = dj, unbias = false, mask_coi = true);

        if (this.f_lowest_freq > 0) {
            w1.setLowestFreq(this.f_lowest_freq);
            w2.setLowestFreq(this.f_lowest_freq);
        }

        //"Transform signals into wavelet domain"
        Eigen::MatrixXd wn1 = w1.wavelet_transform.real;
        Eigen::MatrixXd wn2 = w2.wavelet_transform.real;

        //"Sheinker and Moldwin's Algorithm"
        Eigen::MatrixXd dw = wn2-wn1;
        Eigen::VectorXd wc1 = (dw.array() * wn1.array()).colwise().sum();
        Eigen::VectorXd wc2 = (dw.array() * wn2.array()).colwise().sum();
        Eigen::VectorXd k_hat_real = wc2.array() / wc1.array();
        Eigen::MatrixXd w_clean_real = ((k_hat_real.replicate(wn1.cols(), 1).transpose().array() * wn1.array()) - wn2.array()).array() / (k_hat_real.replicate(wn1.cols(), 1).transpose().array() - 1).array();

        //"Record Scales"
        this.f_scales = w1.scales;

        //"Transform to time domain"
        auto W_n = w_clean_real;
        std::complex<double> Y_00 = w1.wavelet.time(0);
        Eigen::VectorXd s = w1.scales;
        Eigen::VectorXd r_sum = ((W_n.transpose().array() / s.array()) * .5).colwise().sum().transpose();
        Eigen::VectorXd amb_mf = r_sum * (dj * dt *.5 / (w1.C_d * Y_00));
        amb_mf += w1.data.mean(axis = w1.axis, keepdims = True);
        return amb_mf;

    }

    Eigen::VectorXd WAICUP::Multi(MatrixXd signal, float dt, float dj) {
        //"Find Combinations"
        vector<tuple<unsigned int>> pairs = list(itertools.combinations([i for i in range(sig.shape[0])], 2));
        MatrixXd waicup_level1 = np.zeros((len(pairs), sig.shape[-1]));

        vector<WaveletAnalysis> w_obj();
        for i in range(len(pairs)) {
            waicup_level1[i] = dual(np.vstack((sig[pairs[i][0]], sig[pairs[i][1]])), dt, dj);
            WaveletAnalysis wave_obj = WaveletAnalysis(waicup_level1[i], dt = dt, frequency = True, dj = dj);
            wave_obj.lowest_freq = lowest_freq;
            w_obj.append(wave_obj);
        }

        vector<MatrixXd> w = [wav.wavelet_transform.real for wav in w_obj];

        // "Iterate through Level 1 WAICUP"
        w = np.array(w);
        abs_w = np.abs(w);
        indices = np.argmin(abs_w, axis = 0);
        wn_clean = np.take_along_axis(w, indices[None, :, : ], axis = 0)[0];

        // "Reconstruct Ambient Magnetic Field Signal"
        W_n = wn_clean;
        Y_00 = w_obj[0].wavelet.time(0);
        s = w_obj[0].scales;
        r_sum = np.sum(W_n.real.T / s * *.5, axis = -1).T;
        amb_mf = r_sum * (dj * dt * *.5 / (w_obj[0].C_d * Y_00));

        // "Return Ambient Magnetic Field"
        return(amb_mf);
    }
}