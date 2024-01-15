
echo off

set LUA_PATH=;;C:\Dev\repos\C\c_emb_lua\test\?.lua;

pushd build
cemblua_test ..\test\demoapp.lua

popd
