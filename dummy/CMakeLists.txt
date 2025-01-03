cmake_minimum_required(VERSION 3.27)
project(dummy C)

include(FetchContent)

if(ENV{EMROOT} STREQUAL "")
    message(FATAL_ERROR "EMROOT environment variable missing")
endif()

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

message("-- [dummy] C compiler is ${CMAKE_C_COMPILER}")
add_executable(dummy dummy.c)

# Configure platform-specific settings
if(PLATFORM STREQUAL "Web")
    message("-- Configuring for Web platform...")
    # Web-specific configurations
    #set(CMAKE_C_FLAGS -fuse-ld=wasm-ld)
    set(CMAKE_BUILD_TYPE DEBUG)
    #set(CMAKE_SYSTEM_NAME "Emscripten")
    # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
    message("**** $ENV{EMROOT}")

    string(REPLACE "<OBJECT>" "<OBJECT>.bc" generate_bc ${CMAKE_C_COMPILE_OBJECT})
    set(CMAKE_C_COMPILE_OBJECT "${CMAKE_C_COMPILE_OBJECT} && ${generate_bc} -emit-llvm")

    target_compile_options(dummy PUBLIC --target=wasm32 -Wall -emit-llvm -c -Os -DCLAY_WASM -mbulk-memory --no-standard-libraries) # -isysroot $ENV{EMROOT}/cache/sysroot)
    target_link_options(dummy PUBLIC --target=wasm32 -Wl,--export-dynamic -Wl,--export=__heap_base -Wl,--export=ACTIVE_RENDERER_INDEX -Wl,--initial-memory=16908288 -Wl,--no-entry -Wl,--strip-all) # -isysroot $ENV{EMROOT}/cache/sysroot)
    target_link_libraries(dummy $ENV{EMROOT}/cache/sysroot/lib/wasm32-emscripten/crt1.o)
    #target_link_options(dummy PUBLIC --target=wasm32 -Wl,--export-dynamic -Wl,--export=__heap_base -Wl,--export=ACTIVE_RENDERER_INDEX -Wl,--initial-memory=16908288 -Wl,-sWASM=1 -Wl,--no-entry -Wl,-sERROR_ON_UNDEFINED_SYMBOLS=0 -Wl,-sASSERTIONS) #--alow-undefined)
    #target_link_options(dummy PUBLIC -Wl,--export-dynamic) #--alow-undefined)
elseif(PLATFORM STREQUAL "Desktop")
    message("-- Configuring for Desktop platform...")
    # Desktop-specific configurations
    #add_compile_definitions(PLATFORM=PLATFORM_DESKTOP)
    target_compile_options(dummy PUBLIC -Wall -Werror -Wno-unknown-pragmas -DPLATFORM=Desktop -DPLATFORM_DESKTOP)
    target_include_directories(dummy PUBLIC .)
    target_link_libraries(dummy PUBLIC raylib)
endif()

# Adding Clay
set(FETCHCONTENT_QUIET FALSE)
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

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
	cmake_policy(SET CMP0169 OLD)
	FetchContent_Populate(clay)
endif()
include_directories(${clay_SOURCE_DIR})

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
