# Get path to raylib.h
file(GLOB_RECURSE raylib_h ${raylib_SOURCE_DIR} raylib.h)
list(GET raylib_h 0 raylib_h)
get_filename_component(RAYLIB_SRC ${raylib_h} DIRECTORY)
file(WRITE "${CMAKE_BINARY_DIR}/RAYLIB_SRC.txt" "${RAYLIB_SRC}")

# Get path to libraylib.a
file(GLOB_RECURSE raylib_a ${raylib_BINARY_DIR} libraylib.a)
list(GET raylib_a 0 raylib_a)
get_filename_component(RAYLIB_BIN ${raylib_a} DIRECTORY)
file(WRITE "${CMAKE_BINARY_DIR}/RAYLIB_BIN.txt" "${RAYLIB_BIN}")
