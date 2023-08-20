cmake_minimum_required(VERSION 3.15)

project(raycast)

find_package(OpenGL REQUIRED)

include(FetchContent)
include_directories(includes)

FetchContent_Declare(
    stb_image
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)

FetchContent_GetProperties(stb_image)
if(NOT stb_image_POPULATED)
    FetchContent_Populate(stb_image)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_image_SOURCE_DIR})
endif()

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.4
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

add_executable(raycast srcs/Game.cpp srcs/Image.cpp srcs/keyCallback.cpp srcs/main.cpp srcs/Map.cpp srcs/Player.cpp srcs/Sprite.cpp)

target_link_libraries(raycast PRIVATE stb glfw OpenGL::GL)