@echo off
REM Cursor Info Collector Build Script
REM This script automates the build process

echo ========================================
echo Cursor Info Collector - Build Script
echo ========================================
echo.

REM Check if vcpkg is installed
set VCPKG_ROOT=
if exist "D:\vcpkg" (
    set VCPKG_ROOT=D:\vcpkg
) else if exist "%USERPROFILE%\vcpkg" (
    set VCPKG_ROOT=%USERPROFILE%\vcpkg
) else (
    echo Warning: vcpkg not found in common locations
    echo Please install vcpkg or specify SQLite3 path manually
    echo.
    echo To install vcpkg:
    echo   git clone https://github.com/Microsoft/vcpkg.git
    echo   cd vcpkg
    echo   .\bootstrap-vcpkg.bat
    echo   .\vcpkg install sqlite3:x64-windows
    echo   .\vcpkg integrate install
    echo.
    goto :manual_build
)

echo Found vcpkg at: %VCPKG_ROOT%
echo.

REM Check if SQLite3 is installed in vcpkg (static library)
if not exist "%VCPKG_ROOT%\installed\x64-windows-static\include\sqlite3.h" (
    echo SQLite3 static library not found in vcpkg
    echo Installing SQLite3 static library...
    cd /d "%VCPKG_ROOT%"
    call vcpkg install sqlite3:x64-windows-static
    if errorlevel 1 (
        echo Failed to install SQLite3
        pause
        exit /b 1
    )
    cd /d "%~dp0"
)

:vcpkg_build
echo Building with vcpkg (static linking)...
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake (use static triplet)
echo Configuring with CMake for static linking...
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo CMake configuration failed
    cd ..
    pause
    exit /b 1
)

REM Build
echo Building...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed
    cd ..
    pause
    exit /b 1
)

cd ..
goto :success

:manual_build
echo Attempting manual build without vcpkg...
echo Please ensure SQLite3 is installed and accessible
echo.

if not exist build mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo CMake configuration failed
    echo Please install SQLite3 development libraries
    cd ..
    pause
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

:success
echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location:
if exist "build\bin\Release\cursor_info_collector.exe" (
    echo   build\bin\Release\cursor_info_collector.exe
) else if exist "build\Release\cursor_info_collector.exe" (
    echo   build\Release\cursor_info_collector.exe
) else if exist "build\cursor_info_collector.exe" (
    echo   build\cursor_info_collector.exe
) else (
    echo   Could not find executable - please check build output
)
echo.
echo To run the program:
echo   cd build\bin\Release  (or appropriate directory)
echo   .\cursor_info_collector.exe
echo.
pause
