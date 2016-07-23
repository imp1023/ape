bin\rm -fr ../as/com/
bin\rm -fr ../as/protocol-as/*
bin\sed -i s/\/\/package/package/g ./*.proto
bin\protoc23 --plugin=protoc-gen-as3=gen\protoc-gen-as3.bat --as3_out=../as/ -I ./ ./*.proto
bin\sed -i s/package/\/\/package/g ./*.proto
xcopy ..\as\com\sincetimes\redwar\game\comnunicate\protocol\* ..\as\protocol-as /E /R
copy ..\FlashFile\ProtocolMgr.as ..\as\protocol-as\ProtocolMgr.as
bin\rm -fr ../as/com/
pause
