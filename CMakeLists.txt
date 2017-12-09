cmake_minimum_required(VERSION 3.2)
project(AMAZOOM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package( Threads REQUIRED )  # Multithreading package required

# add library directory
include_directories(../library/include)
include_directories(include)
include_directories(./header)

set(HEADERS header/Warehouse.h header/Amazoom.h header/Item.h header/Cell.h header/Coordinate.h header/DeliveryTruck.h
        header/DynamicQueue.h header/Message.h header/Order.h header/RestockTruck.h header/Truck.h header/WarehouseComputer.h header/RemoteWebServer.h)
set(SOURCE_FILES src/Warehouse.cpp src/amazoom_client.cpp src/amazoom_server.cpp)
set(${PROJECT}_FILES ${HEADERS} ${SOURCE_FILES})

#==============================================================
# Amazoom
#==============================================================
add_executable(warehouse src/Warehouse.cpp ${AMAZOOM_HEADERS})
target_link_libraries(warehouse Threads::Threads)

#==============================================================
# Client and server
#==============================================================

add_executable(amazoom_client src/amazoom_client.cpp)
add_executable(amazoom_server src/amazoom_server.cpp)
target_link_libraries(amazoom_server Threads::Threads)