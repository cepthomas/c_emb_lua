:: Standard build - used by VS Code task.

:: Setup dirs and files.
mkdir build
cd build
rem del /F /Q *.*

:: Build the app.
cmake -G "MinGW Makefiles" ..
make
cd ..

rem This really should be done by CMake.
copy source\*.lua build

rem pause
