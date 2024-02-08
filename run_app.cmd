
echo off

set LUA_PATH=;;C:\Dev\repos\C\c_emb_lua\source_code\?.lua;

pushd build_app
cemblua ..\source_code\demoapp.lua

popd
