cmake_minimum_required(VERSION 3.15)
project(BaseProject)

function(build_target_subdir)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build)
		return()
	endif()
	execute_process(COMMAND cmake -B build -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build not found")
	endif()
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/build"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

build_target_subdir(emsdk)
set(ENV{EMROOT} "${CMAKE_CURRENT_SOURCE_DIR}/emsdk/build/emsdk/src/emsdk/upstream/emscripten")
set(ENV{PATH} "$ENV{PATH}:$ENV{EMROOT}")
message("-- Added $ENV{EMROOT} to PATH")

# test if emcc works
execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)

# Install emsdk and update EMROOT and PATH

# Configure project with emsdk
function(add_subdirectory_emcc)
	message("COMMAND ${CMAKE_COMMAND} -DCMAKE_C_COMPILER=clang -DCMAKE_TOOLCHAIN_FILE=$ENV{EMROOT}/cmake/Modules/Platform/Emscripten.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}")
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

# Configure dummy project with Emscripten
add_subdirectory_emcc(dummy)
