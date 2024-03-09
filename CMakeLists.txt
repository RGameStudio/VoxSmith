cmake_minimum_required(VERSION 3.10)

project(VoxSmith)

add_subdirectory(VoxSmith)
set_target_properties(VoxSmith PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(VoxSmith PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)

add_subdirectory(Sandbox)
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Sandbox PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)

target_link_directories(Sandbox PUBLIC ${CMAKE_BINARY_DIR}/VoxSmith/Debug)
add_dependencies(Sandbox VoxSmith)
