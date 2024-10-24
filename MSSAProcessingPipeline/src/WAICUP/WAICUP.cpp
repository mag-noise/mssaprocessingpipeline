"""
Author: Alex Hoffmann
Last Update : 9 / 19 / 2023
Description : Todo

General Parameters
----------
uf : window size for uniform filter used to detrend the data
detrend : boolean for whether to detrend the data

Algorithm Parameters
----------
fs : sampling frequency
dj : wavelet scale spacing
scales : scales used in the wavelet transform(set by the algorithm)
"""

import numpy as np
from wavelets import WaveletAnalysis
from scipy.ndimage import uniform_filter1d
import itertools


#include "WAICUP.hpp"
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

namespace Processor {
    using namespace std;
    using Eigen::MatrixXd;
    int Clean(MatrixXd B_field, bool triaxial) {

        /*
            B : magnetic field measurements from the sensor array(n_sensors, axes, n_samples)
            triaxial : boolean for whether to use triaxial or uniaxial ICA
        */
        

        if (triaxial) :
            result = np.zeros((3, B.shape[-1]))
            for axis in range(3) :
                result[axis] = cleanWAICUP(B[:, axis, : ])
                return(result)
        else:
            // "B: (n_sensors, n_samples)"
            result = cleanWAICUP(B)
            return(result)

    }
    int CleanWAICUP(MatrixXd sensors) {
        dt = 1 / fs

        // "Detrend"
        if (detrend) :
            trend = uniform_filter1d(sensors, size = uf)
            sensors = sensors - trend

        if (sensors.shape[0] == 2) : result = dual(sensors, dt, dj)
        else : result = multi(sensors, dt, dj)

        //"Retrend"
        if (detrend) :
            if (boom is not None) : result += trend[boom]
            else : result += np.mean(trend, axis = 0)

        return(result)

    }

    int Dual(MatrixXd signal, MatrixXd dt, MatrixXd dj) {

        //"Create Wavelets"
        w1 = WaveletAnalysis(sig[0], dt = dt, frequency = True, dj = dj, unbias = False, mask_coi = True)
        w2 = WaveletAnalysis(sig[1], dt = dt, frequency = True, dj = dj, unbias = False, mask_coi = True)

        if (lowest_freq is not None) :
            w1.lowest_freq = lowest_freq
            w2.lowest_freq = lowest_freq


        //"Transform signals into wavelet domain"
        wn1 = w1.wavelet_transform.real
        wn2 = w2.wavelet_transform.real

        //"Sheinker and Moldwin's Algorithm"
        dw = wn2 - wn1
        wc1 = np.sum(dw * wn1, axis = 1)
        wc2 = np.sum(dw * wn2, axis = 1)
        k_hat_real = wc2 / wc1
        w_clean_real = ((np.tile(k_hat_real, (wn1.shape[-1], 1)).T * wn1) - wn2) / (np.tile(k_hat_real, (wn1.shape[-1], 1)).T - 1)

        //"Record Scales"
        global scales
        scales = w1.scales

        //"Transform to time domain"
        W_n = w_clean_real
        Y_00 = w1.wavelet.time(0)
        s = w1.scales
        r_sum = np.sum(W_n.real.T / s * *.5, axis = -1).T
        amb_mf = r_sum * (dj * dt * *.5 / (w1.C_d * Y_00))
        amb_mf += w1.data.mean(axis = w1.axis, keepdims = True)
        return amb_mf

    }
    int Multi(MatrixXd signal, MatrixXd dt, MatrixXd dj) {
        //"Find Combinations"
        pairs = list(itertools.combinations([i for i in range(sig.shape[0])], 2))
        waicup_level1 = np.zeros((len(pairs), sig.shape[-1]))

        w_obj = []
        for i in range(len(pairs)) :
            waicup_level1[i] = dual(np.vstack((sig[pairs[i][0]], sig[pairs[i][1]])), dt, dj)
            wave_obj = WaveletAnalysis(waicup_level1[i], dt = dt, frequency = True, dj = dj)
            wave_obj.lowest_freq = lowest_freq
            w_obj.append(wave_obj)

        w = [wav.wavelet_transform.real for wav in w_obj]

        "Iterate through Level 1 WAICUP"
        w = np.array(w)
        abs_w = np.abs(w)
        indices = np.argmin(abs_w, axis = 0)
        wn_clean = np.take_along_axis(w, indices[None, :, : ], axis = 0)[0]

        "Reconstruct Ambient Magnetic Field Signal"
        W_n = wn_clean
        Y_00 = w_obj[0].wavelet.time(0)
        s = w_obj[0].scales
        r_sum = np.sum(W_n.real.T / s * *.5, axis = -1).T
        amb_mf = r_sum * (dj * dt * *.5 / (w_obj[0].C_d * Y_00))

        "Return Ambient Magnetic Field"
        return(amb_mf)
    }
}