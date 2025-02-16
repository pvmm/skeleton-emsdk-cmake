cmake_minimum_required(VERSION 3.27)

set(PROJECT raygui-app)
project(${PROJECT} C)

message("-- RAYLIB_SRC=${RAYLIB_SRC}")
message("-- RAYLIB_BIN=${RAYLIB_BIN}")

include(FetchContent)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 17)

# Supported platforms
set(SUPPORTED_PLATFORMS "Web" "Desktop") # "Android" "Raspberry")

# Check if the platform is valid
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}. Supported values are: ${SUPPORTED_PLATFORMS}")
endif()

set(TOOL_NAME "\"raygui app\"")
set(TOOL_VERSION "\"0.0.1\"")
set(TOOL_DESCRIPTION "\"Sample raygui app\"")

# Fetch raygui library
FetchContent_Declare(
    raygui
    GIT_REPOSITORY "git@github.com:raysan5/raygui.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(raygui)

# Get path to raygui.h
file(GLOB_RECURSE raygui_h ${raygui_SOURCE_DIR} raygui.h)
list(GET raygui_h 0 raygui_h) # first element of list
get_filename_component(RAYGUI_SRC ${raygui_h} DIRECTORY)
message("-- raygui directory set to ${RAYGUI_SRC}")

file(GLOB app_SRC "*.h" "*.c")
add_executable(${PROJECT} ${app_SRC} external/tinyfiledialogs.c)
target_include_directories(${PROJECT} PUBLIC . ${RAYLIB_SRC} ${RAYGUI_SRC} external)
add_compile_definitions(TOOL_NAME=${TOOL_NAME})
add_compile_definitions(TOOL_VERSION=${TOOL_VERSION})
add_compile_definitions(TOOL_DESCRIPTION=${TOOL_DESCRIPTION})

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message("-- Configuring for Web platform...")
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_SYSTEM_NAME "Emscripten")
    set(HTML_SHELL "${CMAKE_SOURCE_DIR}/shell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_compile_definitions(PLATFORM_WEB)
    add_compile_definitions(_DEFAULT_SOURCE)
    set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "index")
    target_compile_options(${PROJECT} PUBLIC -Wall -std=c99 -Wno-unused-value -Wno-pointer-sign)
    target_link_options(${PROJECT} PUBLIC
        -L. -L${RAYLIB_BIN} -sUSE_GLFW=3 -sTOTAL_MEMORY=128MB -sSTACK_SIZE=1MB -sFORCE_FILESYSTEM=1 -sASYNCIFY
        -sASYNCIFY_STACK_SIZE=1048576 -sASSERTIONS=1 --profiling --shell-file ${HTML_SHELL}
        ${RAYLIB_BIN}/libraylib.a)
    # copy files to web directory
    add_custom_command(
        TARGET ${PROJECT} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
	${CMAKE_CURRENT_BINARY_DIR}/web)
    add_custom_command(
        TARGET ${PROJECT} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/index.html
        ${CMAKE_CURRENT_BINARY_DIR}/index.js
        ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
	${CMAKE_CURRENT_BINARY_DIR}/web)
elseif(PLATFORM STREQUAL "Desktop")
    message("-- Configuring for Desktop platform...")
    set(CMAKE_BUILD_TYPE DEBUG)
    add_compile_definitions(PLATFORM_DESKTOP)
    target_compile_options(${PROJECT} PUBLIC -Wall -Werror -Wno-unknown-pragmas -DPLATFORM=Desktop)
    target_link_libraries(${PROJECT} PUBLIC raylib -lm)
endif()

