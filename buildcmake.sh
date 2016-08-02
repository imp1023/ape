#
#build net
cd net
rm -f CMakeLists.txt
echo "# CMakeLists file">>CMakeLists.txt
echo "add_library(net NetCache.cpp NetHandler.cpp ProtocolHandler.cpp )">>CMakeLists.txt
cd ..

#build common
cd common
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(common  ">>CMakeLists.txt
ls xml/*.cpp>>CMakeLists.txt
ls json/*.cpp>>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..

#build event 
cd event
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(event  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..

#build gamed
cd gamed
rm -f CMakeLists.txt
echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(game  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
ls event/*.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..

#build logic
cd logic
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(logic  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..


#build worldd
cd worldd
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(world  ">>CMakeLists.txt
ls event/*.cpp>>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..

#build platd
cd platd/
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(plat  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
ls event/*.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..

#build country
cd countryd/
rm -f CMakeLists.txt

echo "# CMakeLists file" >>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(country  ">>CMakeLists.txt
ls event/*.cpp>>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo  ") ">>CMakeLists.txt

cd ..
