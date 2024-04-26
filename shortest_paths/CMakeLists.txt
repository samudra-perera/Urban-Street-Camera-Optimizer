cmake_minimum_required(VERSION 2.8.12)
project(shortest_path)  # Changed project name to 'shortest_path'

## Shortest Path Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" ON)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

# create the main executable
## add additional .cpp files if needed
add_executable(shortest_path shortest_path.cpp)

