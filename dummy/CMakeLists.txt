cmake_minimum_required(VERSION 3.15)
project(dummy)

message("-- C compiler set to ${CMAKE_C_COMPILER}")
execute_process(COMMAND emcc --version OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)
