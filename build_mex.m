function build_mex(vcpkg_dir)
    if(~exist('vcpkg_dir', 'var'))
        vcpkg_dir = fullfile(pwd, 'tools', 'vcpkg', 'installed', 'x64-windows' );
    end
    ipath = "-I"+ fullfile(vcpkg_dir, 'include');
    lpath = "-I"+ fullfile(pwd, 'src');
    mex(ipath, lpath, 'src/mexEntry.cpp', 'src/MSSA/MSSA.cpp', 'src/SPU/SPU.cpp');
    
    %ipath = ['-I' fullfile(pwd, 'tools', 'vcpkg', 'installed', 'x64-windows', 'include')];
    %lpath = ['-L' + fullfile(pwd, 'build', 'x64-Debug', "MSSAProcessingPipeline.lib")];
    %lpath = ['-I' fullfile(pwd, 'src')];
    %mex(ipath, lpath, 'src/mexEntry.cpp');
end