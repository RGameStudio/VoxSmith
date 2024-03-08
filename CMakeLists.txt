cmake_minimum_required(VERSION 3.10)

project(VoxSmith)

add_subdirectory(VoxSmith)
set_target_properties(VoxSmith PROPERTIES LINKER_LANGUAGE CXX)
add_subdirectory(Sandbox)
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)

add_dependencies(Sandbox VoxSmith)


