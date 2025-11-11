@echo off
REM Quick run script - builds and runs the collector

echo Building Cursor Info Collector...
call build.bat
if errorlevel 1 (
    echo Build failed, cannot run
    pause
    exit /b 1
)

echo.
echo ========================================
echo Running Cursor Info Collector...
echo ========================================
echo.

REM Find and run the executable
if exist "build\bin\Release\cursor_info_collector.exe" (
    cd build\bin\Release
    cursor_info_collector.exe
    cd ..\..\..
) else if exist "build\Release\cursor_info_collector.exe" (
    cd build\Release
    cursor_info_collector.exe
    cd ..\..
) else if exist "build\cursor_info_collector.exe" (
    cd build
    cursor_info_collector.exe
    cd ..
) else (
    echo Could not find executable
    pause
    exit /b 1
)

echo.
echo ========================================
echo Done! Check the generated JSON file.
echo ========================================
pause
