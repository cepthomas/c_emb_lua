:: Standard build - used by VS Code task.

echo off

:: Setup dirs and files.
mkdir build
cd build
rem del /F /Q *.*

:: Build the app.
cmake -G "MinGW Makefiles" ..
make
cd ..

rem TODO This really should be done by CMake but it's kind of a pain.
copy source\*.lua build

rem pause
