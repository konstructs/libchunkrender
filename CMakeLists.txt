cmake_minimum_required(VERSION 3.5)

include(${CMAKE_SOURCE_DIR}/cmake/astyle.cmake)

project(libChunkRender)

enable_testing()

add_subdirectory(test)
add_subdirectory(libchunkrender)
