cmake_minimum_required(VERSION 3.10)

project(VoxSmith)

add_subdirectory(VoxSmith)
set_target_properties(VoxSmith PROPERTIES 
	CXX_STANDARD 17
	LINKER_LANGUAGE CXX
	CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)


add_subdirectory(Sandbox)
set_target_properties(Sandbox PROPERTIES
	CXX_STANDARD 17
	LINKER_LANGUAGE CXX
	CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(VoxSmith/vendor/GLFW)
add_dependencies(VoxSmith glfw)
target_link_directories(VoxSmith PUBLIC ${CMAKE_BINARY_DIR}/VoxSmith/vendor/GLFW/src/Debug)

target_link_directories(Sandbox PUBLIC ${CMAKE_BINARY_DIR}/VoxSmith/Debug)
add_dependencies(Sandbox VoxSmith)
