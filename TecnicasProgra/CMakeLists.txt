
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(Graphics ${SOURCE_FILES})

file(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")

foreach(FILE_PATH ${ALL_FILES})
    if(IS_DIRECTORY "${FILE_PATH}")
        list(APPEND INCLUDE_DIRS "${FILE_PATH}")
    endif()
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

target_include_directories(Graphics PRIVATE ${INCLUDE_DIRS})