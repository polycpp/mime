cmake_minimum_required(VERSION 3.20)
project(polycpp_mime VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch polycpp (disable its tests when building as a dependency)
set(POLYCPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    polycpp
    GIT_REPOSITORY git@github.com:enricohuang/polycpp.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(polycpp)

# Header-only library
add_library(polycpp_mime INTERFACE)
target_link_libraries(polycpp_mime INTERFACE polycpp)
target_include_directories(polycpp_mime INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Alias for consistent naming
add_library(polycpp::mime ALIAS polycpp_mime)

# Tests
option(POLYCPP_MIME_BUILD_TESTS "Build tests" ON)
if(POLYCPP_MIME_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY git@github.com:google/googletest.git
        GIT_TAG        v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_mime tests/test_mime.cpp)
    target_link_libraries(test_mime PRIVATE polycpp_mime GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(test_mime)
endif()
