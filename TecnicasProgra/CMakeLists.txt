
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(Graphics ${SOURCE_FILES} "src/Graphics/Dx11/Dx11ConstantBuffer.h" "src/Graphics/Dx11/Dx11IndexBuffer.h" "src/Graphics/Dx11/Dx11VertexBuffer.h" "src/Graphics/Dx11/Dx11Topology.h" "src/Graphics/Dx11/Dx11VertexShader.h" "src/Graphics/Dx11/Dx11PixelShader.h" "src/Graphics/Dx11/Dx11DepthStencil.h" "src/Graphics/Dx11/Dx11SwapChain.cpp" "src/Graphics/Dx11/Dx11ViewPort.h" "src/Graphics/RenderTargetView.h" "src/Graphics/Dx11/Dx11RenderTargetView.h")

file(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")

foreach(FILE_PATH ${ALL_FILES})
    if(IS_DIRECTORY "${FILE_PATH}")
        list(APPEND INCLUDE_DIRS "${FILE_PATH}")
    endif()
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

target_include_directories(Graphics PRIVATE ${INCLUDE_DIRS})

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/RawData/Shaders"
    "$<TARGET_FILE_DIR:Graphics>/Shaders"
)