cmake_minimum_required(VERSION 3.10)
project(sound-propagation)

find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -Wextra -Wno-missing-field-initializers")

# Lets the helper code find the floor plan in data/
add_compile_definitions(DATA_DIR="${CMAKE_SOURCE_DIR}/data")

set(SOURCE_FILES
  src/sound.cpp
)

add_executable(serial src/serial.cpp ${SOURCE_FILES})
target_include_directories(serial PRIVATE include)

add_executable(mpi src/mpi.cpp ${SOURCE_FILES})
target_include_directories(mpi PRIVATE include)
target_link_libraries(mpi PRIVATE MPI::MPI_CXX)
