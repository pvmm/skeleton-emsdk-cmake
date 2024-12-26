cmake_minimum_required(VERSION 3.27)
project(dummy C)
set(CMAKE_VERBOSE_MAKEFILE ON)
include(FetchContent)
include(ExternalProject)

set(CMAKE_C_STANDARD 17)

# Define the PLATFORM option
set(PLATFORM "Web" CACHE STRING "Target platform for compilation: web or desktop")

# Supported platforms
set(SUPPORTED_PLATFORMS "Web" "Desktop" "Android" "Raspberry")

# Check if the platform is valid
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}. Supported values are: ${SUPPORTED_PLATFORMS}")
endif()

add_executable(dummy dummy.c)

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message(STATUS "Configuring for Web platform...")
    message(CMAKE_C_COMPILER " ${CMAKE_C_COMPILER}")
    # Web-specific configurations
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_SYSTEM_NAME "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    add_compile_definitions(PLATFORM_WEB)
    add_compile_definitions(PLATFORM=Web)
    target_compile_options(dummy PUBLIC -Os -DCLAY_WASM -mbulk-memory --target=wasm32 -nostdlib)
    add_link_options(--strip-all --export-dynamic --no-entry --export=__heap_base --export=ACTIVE_RENDERER_INDEX --initial-memory=6553600)
elseif(PLATFORM STREQUAL "Desktop")
    message(STATUS "Configuring for Desktop platform...")
    # Desktop-specific configurations
    add_compile_definitions(PLATFORM_DESKTOP)
endif()

# Adding Raylib
set(FETCHCONTENT_QUIET FALSE)
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)
set_target_properties(raylib PROPERTIES CMAKE_C_COMPILER emcc)

FetchContent_Declare(
    clay
    GIT_REPOSITORY "git@github.com:nicbarker/clay.git"
    GIT_TAG "main"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_GetProperties(clay)
if(NOT clay_POPULATED)
  FetchContent_Populate(clay)
endif()
include_directories(${clay_SOURCE_DIR})

#set_target_properties(dummy PROPERTIES CMAKE_C_COMPILER clang)
target_compile_options(dummy PUBLIC -Wall -Werror -Wno-unknown-pragmas)
target_include_directories(dummy PUBLIC .)
target_link_libraries(dummy PUBLIC raylib)
#set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -DCLAY_DEBUG -DPLATFORM")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_custom_command(
        TARGET dummy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources)

