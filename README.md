# Pre-Compiled Usage
## Installation
The MEX File is available for use as a pre-compiled function. This can be found at the [releases page](https://research-git.uiowa.edu/space-physics/epop/mssaprocessingpipeline/-/releases) for download, or in the releases folder when cloning. 
## Usage
To use, unpack the ==mssa.7z== file and include the unpack location in your matlab instance. For example, if unpacking the contents to a folder called "MSSA" use the following commands in Matlab:

```Matlab
% Add the unpacked MEX Function folder
addpath("MSSA");

% Host and run the function on a separate process
mh = mexhost;
[inboard_result, outboard_result] = feval(mh, 'MSSAMex', inboard_input, outboard_input, alpha_threshold, segment_size, window_size);
```

Function inputs are as ordered in the following manner:
1. inboard_input: 3D magnetic field data in 3xN double value format
2. outboard_input: 3D magnetic field data in 3xN double value format
3. alpha_threshold: Value between [0, 1] used to select components for reconstruction
4. segment_size: Size of the N magnetic field data points to use as for each analysis cycle. If the segment_size < N, the magnetic field data is broken up into N/segment_size-1 segments (exludes last segment)
5. window_size: Size of the window used by MSSA when deconstructing signal.

Function outputs are ordered as follows:
1. inboard_result: Reconstructed 3D magnetic field data representing the Inboard data
2. outboard_result: Reconstructed 3D magnetic field data representing the Outboard data

# Compilation Setup (UNFINISHED)
## Requirements
 - C++ Compiler 
   - Microsoft Visual C++ 2022 (C++ Version 17.0) [Download](https://docs.microsoft.com/en-us/visualstudio/releases/2022/release-notes-v17.0)
   - GCC C++ 8.0 Compiler (C++ Version 17.0) 
     - Linux: `sudo apt-get install gcc-c++`
 - CMake (3.15 minimum)
   - Windows: [Download](https://cmake.org/download/)
   - Linux: `sudo apt-get install cmake-build`
 - VCPKG (C++ Package manager)
   - Getting Started via Cloning: [Here](https://vcpkg.io/en/getting-started.html)
 - Matlab installation (Tested on R2022a)
 - Ninja Build (Usually comes with CMake)
   - If needed on linux: `sudo apt-get install ninja-build`

## Building
 - `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake`
   - CMake should install all packages needed, but if not: `%VCPKG_ROOT%\vcpkg install`
 - matlab: %vcpkg_root%
 - matlab: mex -setup
   - mex -setup "C++ Compiler Location"