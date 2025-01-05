cmake_minimum_required(VERSION 3.10)

project(VoxSmith)

add_subdirectory(VoxSmith)
set_target_properties(VoxSmith PROPERTIES 
	CXX_STANDARD 17
	LINKER_LANGUAGE CXX
	CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)
# target_precompile_headers(VoxSmith PRIVATE ${CMAKE_SOURCE_DIR}/VoxSmith/src/PCHVSM.hpp)

add_subdirectory(Sandbox)
set_target_properties(Sandbox PROPERTIES
	CXX_STANDARD 17
	LINKER_LANGUAGE CXX
	CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug-x64)

target_link_directories(Sandbox PUBLIC
	${CMAKE_BINARY_DIR}/VoxSmith/vendor/GLAD/Debug
	${CMAKE_BINARY_DIR}/VoxSmith/vendor/GLFW/src/Debug
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

target_link_directories(VoxSmith PUBLIC 
	${CMAKE_BINARY_DIR}/VoxSmith/vendor/GLAD/Debug
	${CMAKE_BINARY_DIR}/VoxSmith/vendor/GLFW/src/Debug
	${CMAKE_BINARY_DIR}/VoxSmith/vendor/imgui-top-level/Debug)

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR}/Sandbox/)

add_subdirectory(VoxSmith/vendor/GLFW)
add_subdirectory(VoxSmith/vendor/GLAD)
add_subdirectory(VoxSmith/vendor/imgui-top-level)
add_subdirectory(VoxSmith/vendor/glm)

add_dependencies(VoxSmith glad glfw imgui)

target_link_directories(Sandbox PUBLIC ${CMAKE_BINARY_DIR}/VoxSmith/Debug)
add_dependencies(Sandbox VoxSmith)

