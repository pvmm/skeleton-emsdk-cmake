cmake_minimum_required(VERSION 3.15)
project(BaseProject)

function(install_emcc)
	add_subdirectory(emsdk)
	execute_process(COMMAND make
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/emsdk/build")
	set(ENV{EMROOT} "${CMAKE_CURRENT_SOURCE_DIR}/emsdk/build/emsdk/src/emsdk/upstream/emscripten")
	set(ENV{PATH} "$ENV{PATH}:$ENV{EMROOT}")
	message("-- Added $ENV{EMROOT} to path")
	# test if emcc works
	execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)
endfunction()

# Install emsdk and update EMROOT and PATH
install_emcc()

# Configure dummy project with Emscripten
execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_TOOLCHAIN_FILE=$ENV{EMROOT}/cmake/Modules/Platform/Emscripten.cmake .
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dummy"
	COMMAND_ERROR_IS_FATAL ANY)
