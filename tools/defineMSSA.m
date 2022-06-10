%% About defineMSSA.mlx
% This file defines the MATLAB interface to the library |MSSA|.
%
% Commented sections represent C++ functionality that MATLAB cannot automatically define. To include
% functionality, uncomment a section and provide values for &lt;SHAPE&gt;, &lt;DIRECTION&gt;, etc. For more
% information, see <matlab:helpview(fullfile(docroot,'matlab','helptargets.map'),'cpp_define_interface') Define MATLAB Interface for C++ Library>.



%% Setup
% Do not edit this setup section.
function libDef = defineMSSA()
libDef = clibgen.LibraryDefinition("MSSAData.xml");
%% OutputFolder and Libraries 
libDef.OutputFolder = "C:\Users\klsteele\source\repos\mssaprocessingpipeline";
libDef.Libraries = "C:\Users\klsteele\source\repos\mssaprocessingpipeline\build_lib\Debug\MSSAProcessingPipelineLibrary.lib";

%% C++ class |Processor::MSSA| with MATLAB name |clib.MSSA.Processor.MSSA| 
MSSADefinition = addClass(libDef, "Processor::MSSA", "MATLABName", "clib.MSSA.Processor.MSSA", ...
    "Description", "clib.MSSA.Processor.MSSA    Representation of C++ class Processor::MSSA."); % Modify help description values as needed.

%% C++ class constructor for C++ class |Processor::MSSA| 
% C++ Signature: Processor::MSSA::MSSA(Processor::MSSA const & input1)
MSSAConstructor1Definition = addConstructor(MSSADefinition, ...
    "Processor::MSSA::MSSA(Processor::MSSA const & input1)", ...
    "Description", "clib.MSSA.Processor.MSSA Constructor of C++ class Processor::MSSA."); % Modify help description values as needed.
defineArgument(MSSAConstructor1Definition, "input1", "clib.MSSA.Processor.MSSA", "input");
validate(MSSAConstructor1Definition);

%% C++ class constructor for C++ class |Processor::MSSA| 
% C++ Signature: Processor::MSSA::MSSA()
MSSAConstructor2Definition = addConstructor(MSSADefinition, ...
    "Processor::MSSA::MSSA()", ...
    "Description", "clib.MSSA.Processor.MSSA Constructor of C++ class Processor::MSSA."); % Modify help description values as needed.
validate(MSSAConstructor2Definition);

%% C++ class method |Process| for C++ class |Processor::MSSA| 
% C++ Signature: static void Processor::MSSA::Process(std::string filePath)
ProcessDefinition = addMethod(MSSADefinition, ...
    "static void Processor::MSSA::Process(std::string filePath)", ...
    "MATLABName", "Process", ...
    "Description", "Process Method of C++ class Processor::MSSA."); % Modify help description values as needed.
defineArgument(ProcessDefinition, "filePath", "string");
validate(ProcessDefinition);

%% Validate the library definition
validate(libDef);

end
