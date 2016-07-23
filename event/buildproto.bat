..\tools\protoc.exe --cpp_out=. *.proto -I=../ProtoMessage/;../logic/;./

copy *.cc *.cpp /Y

del *.cc
