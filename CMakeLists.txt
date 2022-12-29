cmake_minimum_required(VERSION 3.12)
project(mymuduo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include) 

add_subdirectory(src)

