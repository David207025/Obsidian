cmake_minimum_required(VERSION 3.14)
project(obsidian_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLAD
add_library(glad STATIC
        obsidian_engine/third_party/glad/src/glad.c
        obsidian_engine/include/obsidian.h
        obsidian_engine/source/obsidian.cpp
        obsidian_engine/core.h
        obsidian_engine/source/utils/Graphics.cpp
        obsidian_engine/include/utils/Graphics.h
        obsidian_engine/third_party/stb_setup.h
        obsidian_engine/include/utils/Camera.h
        obsidian_engine/include/utils/Texture.h
        obsidian_engine/source/utils/Texture.cpp
        obsidian_engine/include/includes.h
        obsidian_engine/source/includes.cpp
)
target_include_directories(glad PUBLIC include)

# Add GLFW
add_subdirectory(obsidian_engine/third_party/glfw)

# Your main executable
add_executable(obsidian main.cpp
        obsidian_engine/include/obsidian.h)

# Link GLAD and GLFW to your app
target_link_libraries(obsidian
        glad
        glfw
)

# On macOS, link these too
if(APPLE)
    target_link_libraries(obsidian
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()