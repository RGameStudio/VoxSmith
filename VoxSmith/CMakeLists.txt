cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/VoxSmith/src/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLFW/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLAD/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/glm/)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/imgui-top-level/imgui)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/shaders)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/src/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)
add_library(VoxSmith SHARED ${SOURCE_FILES} ${HEADER_FILES})
add_definitions(-DVS_BUILD_DLL)
target_link_libraries(VoxSmith glfw3.lib glad.lib imgui.lib)
