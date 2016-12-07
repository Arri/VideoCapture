cmake_minimum_required(VERSION 2.8)
project( DisplayMotion )

# Version 1.0
#set (MOTION_VERSION_MAJOR 1)
#set (MOTION_VERSION_MINOR 0)

# Add the header file to the project...
include_directories(./include/)

# Here could manually add the source using the set command as follows:
#set(SOURCES main.cpp camLib.cpp)

# However file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "./include/*.cpp" "./main.cpp")

find_package( OpenCV REQUIRED )
add_executable( displayMotion ${SOURCES} )
target_link_libraries( displayMotion ${OpenCV_LIBS} )
