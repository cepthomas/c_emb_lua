
echo off

:: Setup dirs and files.
mkdir build
pushd build
rem del /F /Q *.*

:: Build the app.
cmake -G "MinGW Makefiles" ..
make

popd

:: This really should be done by CMake.
copy source\lua\*.lua build
