cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/Sandbox/src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLFW/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/GLAD/include)
include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/vendor/glm/)

include_directories(${CMAKE_SOURCE_DIR}/VoxSmith/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Sandbox/src/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)

add_executable(Sandbox ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(Sandbox VoxSmith.lib glad.lib)
