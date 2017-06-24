cmake_minimum_required(VERSION 3.5.1)
project(PoolThread)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES_STACK paralelo/stack/STACK.cpp paralelo/stack/ThreadPool.h paralelo/stack/ThreadPool.cpp)
set(SOURCE_FILES_LOAD paralelo/load/LOAD.cpp paralelo/load/ThreadPool.h paralelo/load/ThreadPool.cpp)
set(SOURCE_FILES_SERIAL sequencial/SERIAL.cpp)

add_executable(STACK ${SOURCE_FILES_STACK})
add_executable(LOAD ${SOURCE_FILES_LOAD})
add_executable(SERIAL ${SOURCE_FILES_SERIAL})

target_link_libraries(STACK -lpthread)
target_link_libraries(LOAD -lpthread)
add_custom_target(run ./run.sh > output.dat)