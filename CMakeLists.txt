cmake_minimum_required(VERSION 3.22)
project(SmartHome C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Get all C files in the source directory
file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.c")
# Remove main.c from the list
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/main.c")

# Add the remaining C files to the library target
add_library(SmartHomeLib ${SOURCE_FILES})

# Add the main.c file to the executable target
add_executable(SmartHome source/main.c)
target_compile_options(SmartHomeLib PRIVATE -gdwarf-4)
target_compile_options(SmartHome PRIVATE -gdwarf-4)
target_link_libraries(SmartHome SmartHomeLib)
