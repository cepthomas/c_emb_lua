
echo off

:: Setup dirs and files.
if not exist build_app mkdir build_app

pushd build_app

:: Build the app.
cmake -G "MinGW Makefiles" ..
make

popd
