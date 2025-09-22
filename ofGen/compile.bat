@echo off
rem ======================================================
rem  Windows batch equivalent of:
rem
rem     mkdir -p build
rem     cd build
rem     cmake ..
rem     cmake --build . --config Release
rem     cmake --install . --config Release
rem ======================================================

rem 1. Create the build directory (mkdir is “do‑nothing if it already exists”).
mkdir build 2>nul

rem 2. Switch into that directory.
pushd build

rem 3. Run CMake to configure the project.
cmake ..

rem 4. Build the project (Release configuration).
cmake --build . --config Release

rem 5. Install the build artefacts (Release configuration).
cmake --install . --config Release

rem 6. Return to the original directory.
popd

rem 7. (Optional) Exit with the same code that the last CMake call produced.
exit /b %ERRORLEVEL%
