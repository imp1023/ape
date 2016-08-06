# CMakeLists file

add_library(plat  
FriendInfoHandler.cpp
GameHandler.cpp
MemCacheServerHandler.cpp
plat.cpp
PlatConfigMgr.cpp
PlatDataHandler.cpp
PlatDataSaver.cpp
platDB.pb.cpp
PlatEventHandler.cpp
PlatNetHandler.cpp
event/BaseEvent.cpp
event/DealPlatEvent.cpp
event/UpdateWorkingStatus.cpp
) 
