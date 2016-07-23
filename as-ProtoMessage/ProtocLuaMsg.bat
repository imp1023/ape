bin\rm -fr ../as/protocol-lua/*
bin\protoc23 --plugin=protoc-gen-lua=gen\protoc-gen-lua.bat --lua_out=../as/protocol-lua -I ./ ./*.proto
copy ..\FlashFile\ProtocolMgr.lua ..\as\protocol-lua\ProtocolMgr.lua
@echo off
@echo on
pause
