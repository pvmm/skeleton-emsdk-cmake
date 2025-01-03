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
	if(${ARGC} EQUAL 0)
		message(FATAL_ERROR "missing argument")
	endif()
	# Remove first parameter and call it TARGET
	set(TARGET "${ARGV0}")
	list(REMOVE_AT ARGV 0)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_TOOLCHAIN_FILE=$ENV{EMROOT}/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_C_COMPILER=emcc ${ARGV} -B build -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/build not found")
	endif()
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/build"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

function(config_clang_target)
	if(${ARGC} EQUAL 0)
		message(FATAL_ERROR "missing argument")
	endif()
	message("CLANG compiler is ${CLANG_EXE}")
	# Remove first parameter and call it TARGET
	set(TARGET "${ARGV0}")
	list(REMOVE_AT ARGV 0)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_C_COMPILER=${CLANG_EXE} ${ARGV} -B build -S . --trace
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/build)
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/build not found")
	endif()
endfunction()

# Configure and build project

# Install emsdk and update EMROOT and PATH
build_target(emsdk)
set(ENV{EMROOT} "${CMAKE_CURRENT_SOURCE_DIR}/emsdk/build/emsdk/src/emsdk/upstream/emscripten")
set(ENV{PATH} "$ENV{PATH}:$ENV{EMROOT}")
set(CMAKE_PROGRAM_PATH $ENV{EMROOT})
message("-- Added $ENV{EMROOT} to PATH")

# Define clang executable
if(DEFINED USE_CLANG)
	find_program(CLANG_EXE NAME clang HINTS $ENV{EMROOT}/../bin REQUIRED)
	message("Checking if clang works... ${CLANG_EXE}")
	execute_process(COMMAND ${CLANG_EXE} --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)
	message("-- Found clang executable at ${CLANG_EXE}")
endif()

# check if emcc works
execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)

# Configure and build raylib
build_emcc_target(raylib -DCMAKE_BUILD_TYPE=DEBUG)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/raylib/build/RAYLIB_SRC.txt" RAYLIB_SRC)
set(ENV{INCLUDE_DIRECTORIES} ${RAYLIB_SRC})
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/raylib/build/RAYLIB_BIN.txt" RAYLIB_BIN)
set(ENV{INCLUDE_DIRECTORIES} ${RAYLIB_BIN})

# Configure raygui-app project with Emscripten
#config_clang_target(raygui-app -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_EXE_LINKER_FLAGS=-L${CMAKE_CURRENT_SOURCE_DIR}/raylib/build/_deps/raylib-build/raylib/)
build_emcc_target(raygui-app -DRAYLIB_SRC=${RAYLIB_SRC} -DRAYLIB_BIN=${RAYLIB_BIN})
