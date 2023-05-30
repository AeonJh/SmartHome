cmake_minimum_required(VERSION 3.22)
project(SmartHome C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCE_FILES source/*.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_executable(SmartHome ${SOURCE_FILES} ${HEADER_FILES})
