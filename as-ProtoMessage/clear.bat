::rm -fr *.proto

::cd ../as/
::rm -fr protocol-as/*
::rm -fr protocol-lua/*
del *.proto
cd ..\as\
del protocol-as\*.as
del protocol-lua\*.lua
pause