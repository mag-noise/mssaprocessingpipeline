ipath = ['-I' fullfile(pwd, 'tools', 'vcpkg', 'installed', 'x64-windows', 'include')];
%lpath = ['-L' + fullfile(pwd, 'build', 'x64-Debug', "MSSAProcessingPipeline.dll")];
lpath = ['-I' fullfile(pwd, 'src')];
mex(ipath, lpath, 'src/mexEntry.cpp', 'src/MSSA/MSSA.cpp', 'src/SPU/SPU.cpp');