#include <c_interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define BUFFER_SIZE 1024
#define MAX_SIZE 3000

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

Test idea: load a known data file used in matlab into here in a similar format, then push shove it through MSSA and expect the same output data

*/
typedef struct MagneticField{
    double* x;
    double* y;
    double* z;

} MagneticField;

MagneticField CreateField(int size) {
    MagneticField mf;
    mf.x = (double*)malloc(size * sizeof(double));
    mf.y = (double*)malloc(size * sizeof(double));
    mf.z = (double*)malloc(size * sizeof(double));
    return mf;
}

void FreeField(MagneticField* mf) {
    free(mf->x);
    free(mf->y);
    free(mf->z);
}

void LoadData(double* boardField, char* fileName) {
    int count = 0;
    FILE* myFile = fopen(fileName, "r");
    // Get Inboard X
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, myFile) && count < MAX_SIZE) {
        // If you need all the values in a row
        char* commaToken = strtok(buffer, ",");
        while (commaToken && count < MAX_SIZE) {
            //printf("%s\n", commaToken);
            // Just printing each integer here but handle as needed
            double n = atof(commaToken);
            boardField[count] = n;
            commaToken = strtok(NULL, ",");
            count++;
        }
    }
    fclose(myFile);

}

void LoadDataLong(long* boardField, char* fileName) {
    int count = 0;
    FILE* myFile = fopen(fileName, "r");
    // Get Inboard X
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, myFile) && count < MAX_SIZE) {
        // If you need all the values in a row
        char* commaToken = strtok(buffer, ",");
        while (commaToken && count < MAX_SIZE) {
            //printf("%s\n", commaToken);
            // Just printing each integer here but handle as needed
            long n = atof(commaToken);
            boardField[count] = n;
            commaToken = strtok(NULL, ",");
            count++;
        }
    }
    fclose(myFile);

}


int main() {
	//process();
    double alpha = 0.05;
    int window = 10;

    const int maxValue = MAX_SIZE;
    MagneticField inboard = CreateField(maxValue);
    MagneticField outboard = CreateField(maxValue);
    MagneticField inboard_out = CreateField(maxValue);
    MagneticField outboard_out = CreateField(maxValue);
    long* t_fit = (long*)malloc(maxValue * sizeof(long));
    
    // Grabbing input data
    LoadData(inboard.x, "data\\inboard_x.csv");
    LoadData(inboard.y, "data\\inboard_y.csv");
    LoadData(inboard.z, "data\\inboard_z.csv");
    LoadData(outboard.x, "data\\outboard_x.csv");
    LoadData(outboard.y, "data\\outboard_y.csv");
    LoadData(outboard.z, "data\\outboard_z.csv");

    LoadDataLong(t_fit, "data\\t_fit.csv");

    double* inboard_full= (double*)malloc(maxValue*3 * sizeof(double));
    double* outboard_full = (double*)malloc(maxValue*3 * sizeof(double));
    double* inboard_result = (double*)malloc(maxValue*3 * sizeof(double));
    double* outboard_result = (double*)malloc(maxValue*3 * sizeof(double));
    double* inboard_wheel = (double*)malloc(maxValue*3 * sizeof(double));
    double* outboard_wheel = (double*)malloc(maxValue*3 * sizeof(double));
    int* flags = (int*)malloc(maxValue*3 * sizeof(int));


    // Put all of the data into the new location
    for (int i = 0; i < maxValue; i++) {
        inboard_full[i] = inboard.x[i];
        inboard_full[i + maxValue] = inboard.y[i];
        inboard_full[i + (maxValue * 2)] = inboard.z[i];
        // We do it
        outboard_full[i] = outboard.x[i];
        outboard_full[i + maxValue] = outboard.y[i];
        outboard_full[i + (maxValue * 2)] = outboard.z[i];
    }


    // Run the function
    process_c(inboard_full, outboard_full, t_fit, maxValue, 3, maxValue*3, 10, 0.05, inboard_result, outboard_result,  inboard_wheel, outboard_wheel, flags);

    // Grabbing results for assertions
    LoadData(inboard_out.x, "data\\inboard_x_output.csv");
    LoadData(inboard_out.y, "data\\inboard_y_output.csv");
    LoadData(inboard_out.z, "data\\inboard_z_output.csv");
    LoadData(outboard_out.x, "data\\outboard_x_output.csv");
    LoadData(outboard_out.y, "data\\outboard_y_output.csv");
    LoadData(outboard_out.z, "data\\outboard_z_output.csv");
    
    printf("inboard values at point 10: [%f,%f,%f]\n", inboard.x[10], inboard.y[10], inboard.z[10]);
    printf("outboard value at point 10: [%f,%f,%f]\n", outboard.x[10], outboard.y[10], outboard.z[10]);
    printf("inboard results at point 10: [%f,%f,%f]\n", inboard_result[10], inboard_result[10+maxValue], inboard_result[10+2*maxValue]);
    printf("outboard results at point 10: [%f,%f,%f]\n", outboard_result[10], outboard_result[10 + maxValue], outboard_result[10 + 2 * maxValue]);
    printf("inboard results at point 10: [%f,%f,%f]\n", inboard_out.x[10], inboard_out.y[10], inboard_out.z[10]);
    printf("outboard results at point 10: [%f,%f,%f]\n", outboard_out.x[10], outboard_out.y[10], outboard_out.z[10]);
    printf("t_fit at point 10: [%d]\n", t_fit[10]);
    
    // Release
    FreeField(&outboard);
    FreeField(&inboard);
    free(t_fit);
    free(inboard_full);
    free(outboard_full);
    free(inboard_result);
    free(outboard_result);
    free(inboard_wheel);
    free(outboard_wheel);
    free(flags);
    
    FreeField(&inboard_out);
    FreeField(&outboard_out);

    return 0;
}