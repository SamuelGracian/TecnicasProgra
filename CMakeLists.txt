cmake_minimum_required(VERSION 3.31)

project(TecnicasProgra)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(TecnicasProgra)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/TecnicasProgra/RawData/Shaders" 
     DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")