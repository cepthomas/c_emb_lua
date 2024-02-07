
echo off

:: Setup dirs and files.
if not exist build mkdir build_test
rem del /F /Q build_test\*.*

pushd build_test

:: Build the app.
cmake -G "MinGW Makefiles" ..
make

popd
