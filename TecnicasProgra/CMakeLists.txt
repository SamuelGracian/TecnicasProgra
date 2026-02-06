file(GLOB SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h)
file(GLOB SOURCE_FILES_1  ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.h)

add_executable(Graphics ${SOURCE_FILES} ${SOURCE_FILES_1})

#includes 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/Dx11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Main)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Display)