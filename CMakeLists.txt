cmake_minimum_required(VERSION 3.2)
project(AMAZOOM)

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# add library directory
include_directories(../library/include)
include_directories(include)

set(AMAZOOM_HEADERS header/*.h)

#==============================================================
# Amazoom
#==============================================================
#add_executable(webserver)
#target_link_libraries()