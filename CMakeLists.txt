cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(MemFileProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(memfile main.cpp)

# Include the header file directory
target_include_directories(memfile PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Enable all compiler warnings
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(memfile PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
