# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(try)
# Set the output folder where your program will be created
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -g -O0")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
#find_package(SDL2 REQUIRED)
#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
# The following folder will be included
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src$
        #{OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS}
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        )

add_executable(${PROJECT_NAME} ${all_SRCS})
#target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image) 
#target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)
#target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )