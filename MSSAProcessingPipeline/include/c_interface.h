#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
/*
* Main function for testing. Entry is the most important part
* void process(
    double* inboardInput
    double* outboardInput
    long* timenum
    unsigned int size
    unsigned int dimensions = 3
    unsigned int inputSize = 0
    unsigned int windowSize = 0
    double alpha=0.05
    double* inboardOutput
    double* outboardOutput
    double* inboardWheel
    double* outboardWheel
    int* flags
)

*/
int say_hi(int number);

int process_c(double* inboardInput, double* outboardInput, long* timenum, unsigned int size,
                double* inboardOutput, double* outboardOutput,
                double* inboardWheel, double* outboardWheel, int* flags,
                unsigned int dimensions, unsigned int inputSize, unsigned int windowSize,
                double alpha);