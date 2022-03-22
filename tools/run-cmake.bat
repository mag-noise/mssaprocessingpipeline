for %%e in (vcpkg.exe) do (set FOUND=%%~$PATH:X)
if defined FOUND (
	echo "Found the file"
)

cmake -B ./build -S . "-DCMAKE_TOOLCHAIN_FILE=C:/Users/klsteele/source/repos/mssaprocessingpipeline/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build ./build