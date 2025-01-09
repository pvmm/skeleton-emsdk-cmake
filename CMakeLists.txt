cmake_minimum_required(VERSION 3.15)
project(BaseProject)

# get build directory name
get_filename_component(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} NAME)

function(configure_target)
	set(TARGET "${ARGV0}")
	list(REMOVE_AT ARGV 0)
	execute_process(COMMAND cmake -B ${BIN_DIR} -S . ${ARGV}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR})
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR} not found")
	endif()
endfunction()

# Configure and build project
function(build_target)
	configure_target(${ARGV})
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/${BIN_DIR}"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

# Configure and build project with emsdk
function(build_emcc_target)
	if(${ARGC} EQUAL 0)
		message(FATAL_ERROR "missing argument: target name")
	endif()
	# Remove first parameter and call it TARGET
	set(TARGET "${ARGV0}")
	list(REMOVE_AT ARGV 0)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_TOOLCHAIN_FILE=$ENV{EMROOT}/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_C_COMPILER=emcc ${ARGV} -B ${BIN_DIR} -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR})
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR} not found")
	endif()
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR}"
		COMMAND_ERROR_IS_FATAL ANY)
endfunction()

function(config_clang_target)
	if(${ARGC} EQUAL 0)
		message(FATAL_ERROR "missing argument: target name")
	endif()
	message("CLANG compiler is ${CLANG_EXE}")
	# Remove first parameter and call it TARGET
	set(TARGET "${ARGV0}")
	list(REMOVE_AT ARGV 0)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_C_COMPILER=${CLANG_EXE} ${ARGV} -B ${BIN_DIR} -S .
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}"
		COMMAND_ERROR_IS_FATAL ANY)
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR})
		message(FATAL_ERROR "directory ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/${BIN_DIR} not found")
	endif()
endfunction()

#
# Configure and build project
#

if(PLATFORM STREQUAL Web)
	set(USE_EMCC ON)
endif()

if(DEFINED USE_EMCC)
	# Install emsdk and update EMROOT and PATH
	build_target(emsdk)
	set(ENV{EMROOT} ${CMAKE_CURRENT_SOURCE_DIR}/emsdk/${BIN_DIR}/emsdk/src/emsdk/upstream/emscripten)
	set(ENV{PATH} "$ENV{PATH}:$ENV{EMROOT}")
	set(CMAKE_PROGRAM_PATH $ENV{EMROOT})
	message("-- Added $ENV{EMROOT} to PATH")

	# check if emcc works
	execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)

	# Configure and build raylib
	build_emcc_target(raylib -DPLATFORM=Web -DCMAKE_BUILD_TYPE=DEBUG)
else()
	# Define clang executables
	find_program(WASM-LD_EXE NAMES wasm-ld)
	if(WASM-LD_EXE STREQUAL "WASM-LD_EXE-NOTFOUND")
		message(FATAL_ERROR "wasm-ld linker not found")
	else()
		message("-- Linker set to ${WASM-LD_EXE}")
	endif()
	find_program(CLANG_EXE NAMES clang)
	if(CLANG_EXE STREQUAL "CLANG_EXE-NOTFOUND")
		message(FATAL_ERROR "clang not found")
	else()
		message("-- Compiler set to ${CLANG_EXE}")
	endif()
	message("-- Checking if clang works... ${CLANG_EXE}")
	execute_process(COMMAND ${CLANG_EXE} --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)
	message("-- Found clang executable at ${CLANG_EXE}")

	# Configure and build raylib
	build_target(raylib -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=DEBUG)
endif()

# Get raylib options back
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/raylib/${BIN_DIR}/RAYLIB_SRC.txt" RAYLIB_SRC)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/raylib/${BIN_DIR}/RAYLIB_BIN.txt" RAYLIB_BIN)

message("PLATFORM=${PLATFORM}")
if(DEFINED USE_EMCC)
	# Configure raygui-app project with Emscripten
	build_emcc_target(raygui-app -DPLATFORM=${PLATFORM} -DRAYLIB_SRC=${RAYLIB_SRC} -DRAYLIB_BIN=${RAYLIB_BIN})
else()
	# Configure raygui-app project with Emscripten
	build_target(raygui-app -DPLATFORM=${PLATFORM} -DRAYLIB_SRC=${RAYLIB_SRC} -DRAYLIB_BIN=${RAYLIB_BIN})
endif()

# Configure clay-app project with Emscripten
if(DEFINED COMPILE_CLAY_SAMPLE)
	config_clang_target(clay-app
		-DCMAKE_VERBOSE_MAKEFILE=ON
		-DCMAKE_EXE_LINKER_FLAGS=-L${CMAKE_CURRENT_SOURCE_DIR}/raylib/${BIN_DIR}/_deps/raylib-build/raylib/
		-DCMAKE_C_FLAGS=-fuse-ld=${WASM-LD_EXE})
	#build_emcc_target(clay-app -DCMAKE_EXE_LINKER_FLAGS=-L${CMAKE_CURRENT_SOURCE_DIR}/raylib/${BIN_DIR}/_deps/raylib-build/raylib/)
endif()
