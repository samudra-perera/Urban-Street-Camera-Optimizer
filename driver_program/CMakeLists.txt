cmake_minimum_required(VERSION 2.8.12)
project(graph_generator)

## Graph Generator Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/run)

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

# create the main executable
## add additional .cpp files if needed
add_executable(shortest_path shortest_path.cpp) 
add_executable(driver driver.cpp)  
add_executable(rgen rgen.cpp)

install(PROGRAMS street_intersection.py DESTINATION bin)  
install(TARGETS shortest_path driver rgen RUNTIME DESTINATION bin)
