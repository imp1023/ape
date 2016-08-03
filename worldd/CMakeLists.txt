# CMakeLists file

add_library(world  
event/BaseEvent.cpp
event/DealAdminEvent.cpp
event/DealWorldEvent.cpp
event/SendRemoteUser.cpp
event/UpdateWorkingStatus.cpp
event/UserLogin.cpp
GameHandler.cpp
WebServerHandler.cpp
WorldDataHandler.cpp
worldd.cpp
WorldEventHandler.cpp
WorldNetHandler.cpp
) 
