cmake_minimum_required(VERSION 3.27)

set(PROJECT clay-app)
project(${PROJECT} C)

include(ExternalProject)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 17)

# Supported platforms
set(SUPPORTED_PLATFORMS "Web" "Desktop") # "Android" "Raspberry")

# Check if the platform is valid
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}. Supported values are: ${SUPPORTED_PLATFORMS}")
endif()

message("-- ${PROJECT}: C compiler is ${CMAKE_C_COMPILER}")
message("-- ${PROJECT}: linker is ${LINKER}")
add_executable(${PROJECT} main.c)

# just download clay, don't do anything else
ExternalProject_Add(clay
    GIT_REPOSITORY "git@github.com:nicbarker/clay.git"
    GIT_TAG "main"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_property(clay SOURCE_DIR)
include_directories(${SOURCE_DIR})
# this step is needed to force the download
add_dependencies(${PROJECT} clay)

# Configure platform-specific settings
if(PLATFORM STREQUAL Web)
    message("-- Configuring for Web platform...")
    # Web-specific configurations
    add_compile_definitions(PLATFORM_WEB)
    set(CMAKE_BUILD_TYPE DEBUG)
    set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "index")
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    #add_compile_definitions(BASE="skeleton-emsdk-cmake/clay-app/")
    # expand @URL@ macro from index.html.in to index.html
    set(URL "https://pvmm.github.io/skeleton-emsdk-cmake/clay-app/")
    #set(URL "http://localhost:8080/")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/index.html.in" INDEX_HTML)
    string(REPLACE @URL@ ${URL} INDEX_HTML "${INDEX_HTML}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/index.html" "${INDEX_HTML}")
    target_compile_options(${PROJECT}
        PUBLIC
        --target=wasm32
        -Wall
        -emit-llvm
        -Os
        -DCLAY_WASM
        -mbulk-memory)
    target_link_options(${PROJECT}
        PUBLIC
        --target=wasm32
        -nostdlib
        -Wl,--strip-all
        -Wl,--export-dynamic
        -Wl,--no-entry
        -Wl,--export=__heap_base
        -Wl,--export=ACTIVE_RENDERER_INDEX
        -Wl,--initial-memory=16908288)
    add_custom_command(
        TARGET ${PROJECT} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory
         ${CMAKE_CURRENT_SOURCE_DIR}/resources
         ${CMAKE_CURRENT_SOURCE_DIR}/../docs/${PROJECT})
     add_custom_command(
         TARGET ${PROJECT} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy
         ${CMAKE_CURRENT_BINARY_DIR}/index.html
         ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
         ${CMAKE_CURRENT_SOURCE_DIR}/../docs/${PROJECT})
elseif(PLATFORM STREQUAL Desktop)
    message("-- Configuring for Desktop platform...")
    add_compile_definitions(PLATFORM_DESKTOP)
    set(CMAKE_BUILD_TYPE DEBUG)
    target_compile_options(${PROJECT}
        PUBLIC
        -Wall
        -Werror
        -Wno-unknown-pragmas)
    target_include_directories(${PROJECT} PUBLIC . ${RAYLIB_SRC})
    target_link_options(${PROJECT} PUBLIC -L${RAYLIB_BIN})
    target_link_libraries(${PROJECT} PUBLIC -lraylib -lm)
endif()

