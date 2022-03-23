folder = cd;
folder = fullfile(folder, '..');
pathlib = fullfile(folder, 'build_lib', 'Debug');
hppFile = 'MSSA.h';
libfile = 'MSSAProcessingPipelineLibrary.lib'

clibgen.generateLibraryDefinition(fullfile(pathlib, hppFile), "Libraries", fullfile(pathlib, libfile), "PackageName", "MSSA", "ReturnCArrays", false, "OverwriteExistingDefinitionFiles", true);