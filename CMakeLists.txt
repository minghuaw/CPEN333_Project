cmake_minimum_required(VERSION 3.2)
project(AMAZOOM)

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# add library directory
include_directories(../library/include)
# add json support
include_directories(include)
# access to src/ files for tests
include_directories(src)
include_directories(header)


# copy data files
FILE(COPY data DESTINATION "${CMAKE_BINARY_DIR}")
FILE(COPY data DESTINATION "${CMAKE_BINARY_DIR}/Debug")

set(AMAZOOM_HEADERS 
	header/Warehouse.h 
	header/Amazoom.h 
	header/Item.h header/Cell.h 
	header/Coordinate.h 
	header/DeliveryTruck.h
	header/DynamicQueue.h 
	header/Message.h 
	header/Order.h 
	header/RestockTruck.h 
	header/Truck.h 
	header/WarehouseComputer.h
	header/UserInterface.h
	header/WarehouseComputerAPI.h
	header/Quote.h
	header/JsonConverter.h
	header/InventoryDatabase.h
	header/Layout.h
	header/safe_printf.h
	header/Robot.h
	header/Truck.h
	include/json.hpp)

#==============================================================
# Amazoom
#==============================================================
add_executable(warehouse src/Warehouse.cpp ${AMAZOOM_HEADERS})
add_executable(amazoom_client src/amazoom_client.cpp ${AMAZOOM_HEADERS})
add_executable(amazoom_server src/amazoom_server.cpp ${AMAZOOM_HEADERS})
target_link_libraries(warehouse Threads::Threads)

add_executable(LayoutGUI src/LayoutGUI.cpp ${AMAZOOM_HEADERS})
target_link_libraries(LayoutGUI Threads::Threads)

if(WIN32)
    target_link_libraries(warehouse wsock32 ws2_32)
	target_link_libraries(amazoom_client wsock32 ws2_32)
	target_link_libraries(amazoom_server wsock32 ws2_32)
    target_link_libraries(LayoutGUI wsock32 ws2_32)
endif()