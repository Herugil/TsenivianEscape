cmake_minimum_required(VERSION 3.20)
project(TsenivianEscape)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Auto-find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(game ${SOURCES})

# Include directories
target_include_directories(game PRIVATE src)