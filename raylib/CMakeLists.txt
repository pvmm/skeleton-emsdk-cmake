cmake_minimum_required(VERSION 3.27)
project(raylib_ C)
set(CMAKE_VERBOSE_MAKEFILE ON)
include(FetchContent)
include(ExternalProject)
set(FETCHCONTENT_QUIET FALSE)

# Define the PLATFORM option
set(PLATFORM "Web" CACHE STRING "Target platform for compilation: Web or Desktop")

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
    # Desktop-specific configurations
    message("-- Configuring for Desktop platform...")
    add_compile_definitions(PLATFORM_DESKTOP)
    set_target_properties(raylib PROPERTIES CMAKE_VERBOSE_MAKEFILE ON)
endif()

# fake target forces raylib to compile
add_executable(raylib_ tmp.c)
target_link_libraries(raylib_ PUBLIC raylib)

add_custom_command(
    TARGET raylib_ POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -D raylib_SOURCE_DIR=${raylib_SOURCE_DIR} -D raylib_BINARY_DIR=${raylib_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/vars.cmake)
