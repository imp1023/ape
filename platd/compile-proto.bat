protoc.exe --cpp_out=. platDB.proto

copy *.cc *.cpp /Y

del *.cc