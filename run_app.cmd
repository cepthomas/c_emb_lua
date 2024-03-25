
echo off

set LUA_PATH=;;%~dp0source_code\?.lua;

pushd build_app
cemblua ..\source_code\demoapp.lua

popd
