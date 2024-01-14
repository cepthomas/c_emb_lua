
echo off

pushd build
rem TODO-T crashes:
cemblua_test demoapp.lua
popd
