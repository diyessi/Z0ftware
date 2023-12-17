cmake_minimum_required(VERSION 3.26.3)

set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

project(Z0ftware VERSION 0.1.0)

configure_file(include/Z0ftware/config.h.in include/Z0ftware/config.h)
include_directories(${PROJECT_BINARY_DIR}/include)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
if((NOT DEFINED ENV{LLVM_DIR}) AND ${APPLE})
    message(STATUS "Using homebrew llvm")
    EXECUTE_PROCESS(
        COMMAND brew --prefix llvm
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE LLVM_DIR
    )
    set(ENV{LLVM_DIR} ${LLVM_DIR})
    message(STATUS "Using homebrew LLVM_DIR: ${LLVM_DIR}")
endif()

# From https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

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

add_subdirectory(sap23)
