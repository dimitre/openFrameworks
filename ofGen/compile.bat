@echo off
rem -------------------------------------------------------------
rem  build_and_install.bat
rem  - Builds the project in Release mode
rem  - If a protected install location (e.g. Program Files) is
rem    used, the script re‑launches itself as Administrator
rem    *before* running `cmake --install`.
rem -------------------------------------------------------------

rem -------------------------------------------------------------
rem 1.  Detect whether we already have admin rights
rem -------------------------------------------------------------
>nul 2>&1 (
    net session >nul
)
if errorlevel 1 (
    rem ---------------------------------------------------------
    rem 2.  We are NOT admin – re‑launch ourselves as admin
    rem ---------------------------------------------------------
    echo Requesting administrator privileges...
    powershell -NoProfile -Command ^
        "Start-Process -FilePath '%~f0' -Verb RunAs -ArgumentList @(%*)"
    exit /b
)

rem -------------------------------------------------------------
rem 3.  Normal (admin) mode – do the real work
rem -------------------------------------------------------------
rem  3a. Create the build dir (mkdir is safe if it already exists)
mkdir build 2>nul

rem  3b. Switch to that dir
pushd build

rem  3c. Configure
cmake ..

rem  3d. Build
cmake --build . --config Release

rem  3e. Install – this will run under admin privileges
cmake --install . --config Release

rem  3f. Return to caller dir
popd

rem  3g. Exit with the same status code as the last command
exit /b %ERRORLEVEL%
