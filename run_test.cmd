
echo off

set LUA_PATH=;;C:\Dev\repos\C\c_emb_lua\test_code\?.lua;

pushd build_test
cemblua_test ..\test_code\demoapp.lua

popd
