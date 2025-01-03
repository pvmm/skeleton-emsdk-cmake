cmake_minimum_required(VERSION 3.27)
project(raygui-app C)
message("-- RAYLIB_SRC=${RAYLIB_SRC}")
message("-- RAYLIB_BIN=${RAYLIB_BIN}")

include(FetchContent)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 17)

# Define the PLATFORM option
set(PLATFORM "Web" CACHE STRING "Target platform for compilation: web or desktop")

# Supported platforms
set(SUPPORTED_PLATFORMS "Web" "Desktop") # "Android" "Raspberry")

# Check if the platform is valid
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}. Supported values are: ${SUPPORTED_PLATFORMS}")
endif()

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
FetchContent_GetProperties(raygui)
if(NOT raygui_POPULATED)
	#cmake_policy(SET CMP0169 OLD)
	FetchContent_Populate(raygui)
endif()

add_executable(raygui-app main.c external/tinyfiledialogs.c)
message("source_dir ${raygui_SOURCE_DIR}/src")
target_include_directories(raygui-app PUBLIC ${raygui_SOURCE_DIR}/src)

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message("-- Configuring for Web platform...")
    # Web-specific configurations
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_SYSTEM_NAME "Emscripten")
    # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set(HTML_SHELL "${CMAKE_SOURCE_DIR}/shell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_compile_definitions(PLATFORM=PLATFORM_WEB)
    add_compile_definitions(PLATFORM_WEB)
    add_compile_definitions(_DEFAULT_SOURCE)
    target_compile_options(raygui-app PUBLIC -Wall -std=c99 -Wno-missing-braces
        -Wno-unused-value -Wno-pointer-sign -I. -I${RAYLIB_SRC} -Iexternal)
    target_link_options(raygui-app PUBLIC
        -L. -L${RAYLIB_BIN} -sUSE_GLFW=3 -sTOTAL_MEMORY=128MB -sSTACK_SIZE=1MB -sFORCE_FILESYSTEM=1 -sASYNCIFY
        -sASYNCIFY_STACK_SIZE=1048576 -sASSERTIONS=1 --profiling --shell-file ${HTML_SHELL}
        ${RAYLIB_BIN}/libraylib.a)
elseif(PLATFORM STREQUAL "Desktop")
    message("-- Configuring for Desktop platform...")
    # Desktop-specific configurations
    add_compile_definitions(PLATFORM=PLATFORM_DESKTOP)
    target_compile_options(raygui-app PUBLIC -Wall -Werror -Wno-unknown-pragmas -DPLATFORM=Desktop)
    target_link_libraries(raygui-app PUBLIC raylib)
endif()

add_custom_command(
	TARGET raygui-app POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
	${CMAKE_CURRENT_BINARY_DIR}/web)

add_custom_command(
	TARGET raygui-app POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/resources
	${CMAKE_CURRENT_BINARY_DIR}/web)

add_custom_command(
	TARGET raygui-app POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_BINARY_DIR}/raygui-app.html
	${CMAKE_CURRENT_BINARY_DIR}/raygui-app.js
	${CMAKE_CURRENT_BINARY_DIR}/raygui-app.wasm
	${CMAKE_CURRENT_BINARY_DIR}/web/)

