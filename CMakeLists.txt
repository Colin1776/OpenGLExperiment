cmake_minimum_required(VERSION 3.24)
project(OpenGLExperiment)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(OpenGLExperiment src/main.cpp)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory("library/glfw-3.3.8")
add_subdirectory("library/glm")

target_link_libraries(OpenGLExperiment glfw)
target_link_libraries(OpenGLExperiment glm)

include_directories(res/shaders)
include_directories(res/textures)
include_directories(include)

add_compile_options(-O3)

