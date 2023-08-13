cmake_minimum_required(VERSION 3.26.3)

set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

project(Z0ftware VERSION 0.1.0)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include(FetchContent)
include(CTest)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# This is overkill for parsing expressions
FetchContent_Declare(
    cpp-peglib
    GIT_REPOSITORY https://github.com/yhirose/cpp-peglib.git
    GIT_TAG 86208ed
)

option(cpp-peglib_BUILD_TESTS OFF)

FetchContent_MakeAvailable(cpp-peglib)

add_subdirectory(lib)
add_subdirectory(tests)
enable_testing()
