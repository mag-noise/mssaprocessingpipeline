function build_mex(vcpkg_dir)
    if(~exist('vcpkg_dir', 'var'))
        vcpkg_dir = fullfile(pwd, 'tools', 'vcpkg', 'installed', 'x64-windows' );
    end
    ipath = "-I"+ fullfile(vcpkg_dir, 'include');
    %lpath = "-I"+ fullfile(pwd, 'src');
    %mex(ipath, lpath, 'src/mexEntry.cpp', 'src/MSSA/MSSA.cpp', 'src/SPU/SPU.cpp', '-output','MSSAMex', '-outdir', 'releases');
    
    ipath = "-I"+ fullfile(pwd, "releases")
    Lpath = "-L" + fullfile(pwd, "releases");
    lpath = "-l" +  fullfile(pwd, "releases", "MSSAProcessingPipeline.lib");
    lpath = "-l" + "MSSAProcessingPipeline.lib";

    %lpath = ['-I' fullfile(pwd, 'src')];
    mex(ipath, lpath, Lpath, 'src/mexEntry.cpp');
end