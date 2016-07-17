..\tools\protoc.exe --cpp_out=. dbinterface.proto -I=../ProtoMessage/;./

copy *.cc *.cpp /Y

del *.cc

pause
