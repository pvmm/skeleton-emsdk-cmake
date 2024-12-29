cmake_minimum_required(VERSION 3.27)
project(raygui-app C)
message("-- [raygui-app] RAYLIB_SRC=${RAYLIB_SRC}")

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

add_executable(raygui-app main.c)
message("source_dir ${raygui_SOURCE_DIR}/src")
target_include_directories(raygui-app PUBLIC ${raygui_SOURCE_DIR}/src)

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message("-- Configuring for Web platform...")
    # Web-specific configurations
    set(CMAKE_BUILD_TYPE RELEASE)
    #set(CMAKE_SYSTEM_NAME "Emscripten")
    # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    #add_compile_definitions(PLATFORM=PLATFORM_WEB)
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
    target_compile_options(raygui-app PUBLIC --target=wasm32 -Wall -Os -DCLAY_WASM -mbulk-memory -nostdlib -I${RAYLIB_SRC})
    target_link_options(raygui-app PUBLIC --strip-all --export-dynamic --export=__heap_base --export=ACTIVE_RENDERER_INDEX --initial-memory=16908288 -s WASM=1 --no-entry)
elseif(PLATFORM STREQUAL "Desktop")
    message("-- Configuring for Desktop platform...")
    # Desktop-specific configurations
    #add_compile_definitions(PLATFORM=PLATFORM_DESKTOP)
    target_compile_options(raygui-app PUBLIC -Wall -Werror -Wno-unknown-pragmas -DPLATFORM=Desktop -DPLATFORM_DESKTOP)
    target_link_libraries(raygui-app PUBLIC raylib)
endif()

# Adding Clay
set(FETCHCONTENT_QUIET FALSE)
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

message("-- [raygui-app] C compiler is ${CMAKE_C_COMPILER}")

#add_custom_command(
#	TARGET dummy POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E make_directory
#	${CMAKE_CURRENT_BINARY_DIR}/web)

#add_custom_command(
#	TARGET dummy POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#	${CMAKE_CURRENT_SOURCE_DIR}/resources
#	${CMAKE_CURRENT_BINARY_DIR}/web)

#add_custom_command(
#	TARGET dummy POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy
#	${CMAKE_CURRENT_BINARY_DIR}/dummy.wasm
#	${CMAKE_CURRENT_BINARY_DIR}/web/index.wasm)
