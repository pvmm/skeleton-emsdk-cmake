cmake_minimum_required(VERSION 3.27)
project(build_raylib C)
set(CMAKE_VERBOSE_MAKEFILE ON)
include(FetchContent)
include(ExternalProject)
set(FETCHCONTENT_QUIET FALSE)

# Supported platforms
set(SUPPORTED_PLATFORMS "Web" "Desktop") # "Android" "Raspberry")

# Check if the platform is valid
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}. Supported values are: ${SUPPORTED_PLATFORMS}")
endif()

message("-- [raylib] C compiler is ${CMAKE_C_COMPILER}")

# Adding raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message("-- Configuring for Web platform...")
    if(NOT EXISTS $ENV{EMROOT})
        message(FATAL_ERROR "EMROOT environment variable not found")
    endif()
    set(ENV{PATH} "$ENV{PATH}:${EMROOT}")
    set_target_properties(raylib PROPERTIES CMAKE_VERBOSE_MAKEFILE ON
        CMAKE_TOOLCHAIN_FILE ${EMROOT}/cmake/Modules/Platform/Emscripten.cmake
        CMAKE_C_COMPILER emcc)
elseif(PLATFORM STREQUAL "Desktop")
    message("-- Configuring for Desktop platform...")
    add_compile_definitions(PLATFORM_DESKTOP)
    set_target_properties(raylib PROPERTIES CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Custom target forces raylib to compile
add_custom_target(build_raylib ALL DEPENDS raylib)

# Send raylib variables upstream to the root project
add_custom_command(
    TARGET build_raylib POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -D raylib_SOURCE_DIR=${raylib_SOURCE_DIR} -D raylib_BINARY_DIR=${raylib_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/vars.cmake)
