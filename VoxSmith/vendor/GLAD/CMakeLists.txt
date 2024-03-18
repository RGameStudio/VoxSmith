cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLAD/include/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLAD/src/glad.c)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLAD/include/)

add_library(glad STATIC ${SOURCE_FILES} ${HEADER_FILES})
