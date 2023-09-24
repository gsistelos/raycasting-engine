cmake_minimum_required(VERSION 3.25.1)

project(raycast)

# opengl
find_package(OpenGL REQUIRED)

# glfw
add_subdirectory(glfw)

# stb_image
file(DOWNLOAD https://raw.githubusercontent.com/nothings/stb/master/stb_image.h ${CMAKE_SOURCE_DIR}/include/stb_image.h)

set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror -Wextra -O3)

include_directories(include)

add_executable(
    raycast
    src/Game.cpp
    src/Image.cpp
    src/keyCallback.cpp
    src/main.cpp
    src/Map.cpp
    src/Player.cpp
    src/Sprite.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

target_link_libraries(raycast glfw OpenGL::GL)
