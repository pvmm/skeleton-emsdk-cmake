cmake_minimum_required(VERSION 3.15)
project(BaseProject)

function(configure_target)
	execute_process(COMMAND cmake -B build -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build not found")
	endif()
endfunction()

# Configure and build project
function(build_target)
	configure_target(${ARGV0})
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

# Configure and build project with emsdk
function(build_emcc_target)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_TOOLCHAIN_FILE=$ENV{EMROOT}/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_C_COMPILER=emcc -B build -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build not found")
	endif()
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

function(configure_clang_target)
	message("CLANG_EXE ${CLANG_EXE}")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_C_COMPILER=${CLANG_EXE} -B build -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build not found")
	endif()
endfunction()

# Configure and build project

# Install emsdk and update EMROOT and PATH
build_target(emsdk)
set(ENV{EMROOT} "${CMAKE_CURRENT_SOURCE_DIR}/emsdk/build/emsdk/src/emsdk/upstream/emscripten")
set(ENV{PATH} "$ENV{PATH}:$ENV{EMROOT}")
message("-- Added $ENV{EMROOT} to PATH")
find_program(CLANG_EXE NAME clang PATH_SUFFIXES $ENV{EMROOT})
if(CLANG_EXE STREQUAL "CLANG_EXE-NOTFOUND")
	message(FATAL_ERROR "emsdk clang not found")
endif()
message("-- Found clang executable at ${CLANG_EXE}")
# check if emcc works
execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)

# Configure and build raylib
build_emcc_target(raylib)

# Configure dummy project with Emscripten
configure_clang_target(dummy)
