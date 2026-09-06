cmake_minimum_required(VERSION 3.10)
project(lecture-code)

find_package(OpenMP)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -Wextra -Wno-missing-field-initializers")

add_executable(upsample src/upsample.cpp)
target_include_directories(upsample PRIVATE include)
target_link_libraries(upsample PUBLIC OpenMP::OpenMP_CXX)

add_executable(blur src/blur.cpp)
target_include_directories(blur PRIVATE include)
target_link_libraries(blur PUBLIC OpenMP::OpenMP_CXX)

add_executable(threshold src/threshold.cpp)
target_include_directories(threshold PRIVATE include)
target_link_libraries(threshold PUBLIC OpenMP::OpenMP_CXX)
