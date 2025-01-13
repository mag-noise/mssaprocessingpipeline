#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for copy() and assign()�
#include <iterator> // for back_inserter�

Utils::FlagSystem* Utils::FlagSystem::instance;
Utils::Injector* Utils::Injector::instance;

/// <summary>
/// Entry function for the library. Meant to be exposed and utilized by any program that has access to C++ libraries.
/// Will need an extern "C" wrapper in order to transfer out, or a complete wrapper like MSSAPython or mexEntry
/// Input array must be organized as the following: Assuming a 4 dimensional input, [a, b, c, d, a, b, c, d, ... ]
/// 'size' parameter == number of points in timenum == number of points in 1 dimension of an input array
/// Input array must be a rectangular array (i.e. all dimensions are the same size)
/// </summary>
void process(double* inboardInput, double* outboardInput, long* timenum, unsigned int size, 
    double* inboardOutput, double* outboardOutput,
    double* inboardWheel, double* outboardWheel, int* flags,
    unsigned int dimensions = 3, unsigned int inputSize = 0, unsigned int windowSize = 0,
    double alpha=0.05) {
    using SignalProcessingUnit::MSSAProcessingUnit;

    // These values basically tell the program how to interpret the input arrays.
    // Pretty much all arrays, including outputs, use these values to interpret structure
    if (size % dimensions != 0) {
        return;
    }

    // Has automatic setup without these values, so basically only re-initialize them if the new sizes are asked for
    if (windowSize > 0 && inputSize > 0)
    {
        Processor::MSSA::DynamicVariableSetup(/*Input Size*/inputSize, /*Window Size*/windowSize);
    }
    else {
        Processor::MSSA::DynamicVariableSetup(/*Input Size*/size, /*Window Size*/10);

    }

    MSSAProcessingUnit<double> inboard = MSSAProcessingUnit<double>(true, dimensions);
    MSSAProcessingUnit<double> outboard = MSSAProcessingUnit<double>(false, dimensions);

    //Checks the inputs for any initial flags that can cause issues downstream
    std::vector<double> dest(inboardInput, inboardInput + (size * dimensions));
    std::vector<double> dest2(outboardInput, outboardInput + (size * dimensions));

    Utils::FlagSystem::GetInstance()->Resize(dest.size());
    Utils::FlagSystem::GetInstance()->FlagNaN(dest);
    Utils::FlagSystem::GetInstance()->FlagNaN(dest2);

    std::vector<double> timevec(timenum, timenum+ size);
    Utils::FlagSystem::GetInstance()->FlagDiscontinuity(timevec);

    // Main Processing area
    inboard.PreProcess(dest, true);
    outboard.PreProcess(dest2, true);

    std::vector<double> alpha_val = { alpha, alpha, alpha };

    
    MSSAProcessingUnit<double>::Process(inboard, outboard, alpha_val);

    // Due to how the failed_wheel is found in the program, this ensures that the flag is correctly raised prior 
    // to committing joined containers
    auto temp = outboard.JoinWheel();
    auto wheel = inboard.JoinWheel();
    for (int i = 0; i < size; i++) {
        inboardWheel[i] = wheel[i];
    }

    temp = outboard.JoinWheel();
    for (int i = 0; i < size; i++) {
        outboardWheel[i] = temp[i];
    }


    // Saving signals to output pointers. Unsure if this will be be reference or by value when moving from signal variable to output
    auto signal = inboard.JoinSignal(dest);

    auto signal2 = outboard.JoinSignal(dest2);
    std::vector<int> temp2 = Utils::FlagSystem::GetInstance()->Snapshot();
    for (int i = 0; i < size; i++) {
        inboardOutput[i] = signal[i];
        outboardOutput[i] = signal2[i];
        flags[i] = temp2[i];
    }
    inboard.CheckSignalDifference(signal, wheel, dest);
    outboard.CheckSignalDifference(signal2, temp, dest2);

}


extern "C"{
    #include "../include/c_interface.h"

    int process_c(double* inboardInput, double* outboardInput, long* timenum, unsigned int size,
        double* inboardOutput, double* outboardOutput,
        double* inboardWheel, double* outboardWheel, int* flags,
        unsigned int dimensions, unsigned int inputSize, unsigned int windowSize,
        double alpha) {

        if (alpha < 0) return -1;

        process(inboardInput, outboardInput, timenum, size,
            inboardOutput, outboardOutput,
            inboardWheel, outboardWheel, flags,
            dimensions, inputSize, windowSize, alpha);
        return 0;
    }

    int say_hi(int number) {
        printf("Hello number %d. Adding 10 to you...", number);
        return number + 10;
    }
}