cmake_minimum_required(VERSION 3.20)
project(TsenivianEscape)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force libc++ on macOS
if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -Wall -Wextra -Wconversion -Wsign-conversion -Werror")
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(game src/main.cpp ${SOURCES})

# Include directories
target_include_directories(game PRIVATE src)

find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS random)
target_link_libraries(game PRIVATE nlohmann_json::nlohmann_json Boost::random)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(runTests ${TEST_SOURCES} ${SOURCES})
target_include_directories(runTests PRIVATE src)
target_link_libraries(runTests PRIVATE gtest gtest_main nlohmann_json::nlohmann_json Boost::random)

# google test triggers some sign-conversion warnings, disable them
target_compile_options(runTests PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-sign-conversion>
)
enable_testing()
add_test(NAME AllTests COMMAND runTests)