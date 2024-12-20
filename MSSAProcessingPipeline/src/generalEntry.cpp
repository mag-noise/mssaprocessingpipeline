#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for copy() and assign()�
#include <iterator> // for back_inserter�

Utils::FlagSystem* Utils::FlagSystem::instance;

/// <summary>
/// Entry function for the library. Meant to be exposed and utilized by any program that has access to C++ libraries.
/// Will need an extern "C" wrapper in order to transfer out, or a complete wrapper like MSSAPython or mexEntry
/// </summary>
void process(double* inboardInput, double* outboardInput, long* timenum, unsigned int size, 
    unsigned int dimensions = 3, unsigned int inputSize = 0, unsigned int windowSize = 0,
    double alpha=0.05, double* inboardOutput, double* outboardOutput, 
    double* inboardWheel, double* outboardWheel, int* flags) {
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
