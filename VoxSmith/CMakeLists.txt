cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/src/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/src/)

add_definitions(-DVS_BUILD_DLL)
add_library(VoxSmith SHARED ${SOURCE_FILES} ${HEADER_FILES})
