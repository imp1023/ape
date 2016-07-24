protoc.exe --cpp_out=. countryDB.proto

copy *.cc *.cpp /Y

del *.cc