cmake_minimum_required(VERSION 3.10)
project(webcrawler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -Wextra")

set(SOURCE_FILES 
  src/net.cpp
)

add_executable(serial src/serial.cpp ${SOURCE_FILES})
target_include_directories(serial PRIVATE include)

add_executable(threaded src/threaded.cpp ${SOURCE_FILES})
target_include_directories(threaded PRIVATE include)
target_link_libraries(threaded PRIVATE pthread)
