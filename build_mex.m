function build_mex(vcpkg_dir)
    if(~exist('vcpkg_dir', 'var'))
        vcpkg_dir = fullfile(pwd, 'tools', 'vcpkg', 'installed', 'x64-windows' );
    end
    ipath = "-I"+ fullfile(vcpkg_dir, 'include');
    lpath = "-I"+ fullfile(pwd, 'src');
    mex(ipath, lpath, 'src/mexEntry.cpp', 'src/MSSA/MSSA.cpp', 'src/SPU/SPU.cpp', '-output','MSSAMex', '-outdir', 'release/mssa', '-g');
    rmpath(fullfile("build","x64-Release", "mssa"))
    rmpath(fullfile("build","x64-Debug", "mssa"))

    %ipath = "-I"+ fullfile(pwd, "releases", "mssa")
    %Lpath = "-L" + fullfile(pwd, "build", "x64-Debug", "mssa");
    %lpath = "-l" +  fullfile(pwd, "releases", "MSSAProcessingPipeline.lib");
    %lpath = "-l" + "MSSAProcessingPipeline.lib";

    %lpath = ['-I' fullfile(pwd, 'src')];
    %mex(ipath, lpath, Lpath, 'src/mexEntry.cpp');
end