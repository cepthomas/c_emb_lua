
echo off

:: Setup dirs and files.
mkdir build
rem del /F /Q build\*.*

pushd build

:: Build the app.
cmake -G "MinGW Makefiles" ..
make

popd
