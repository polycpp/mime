cmake_minimum_required(VERSION 3.20)
project(polycpp_mime VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch polycpp (disable its tests when building as a dependency)
set(POLYCPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

if(DEFINED POLYCPP_SOURCE_DIR)
    FetchContent_Declare(polycpp SOURCE_DIR "${POLYCPP_SOURCE_DIR}")
else()
    FetchContent_Declare(
        polycpp
        GIT_REPOSITORY https://github.com/enricohuang/polycpp.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
endif()
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

set(POLYCPP_MIME_BUILD_TESTS_DEFAULT OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(POLYCPP_MIME_BUILD_TESTS_DEFAULT ON)
endif()
option(POLYCPP_MIME_BUILD_TESTS "Build tests" ${POLYCPP_MIME_BUILD_TESTS_DEFAULT})
if(POLYCPP_MIME_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(polycpp_mime_test_mime tests/test_mime.cpp)
    target_link_libraries(polycpp_mime_test_mime PRIVATE polycpp::mime GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(polycpp_mime_test_mime)
endif()

set(POLYCPP_MIME_BUILD_EXAMPLES_DEFAULT OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(POLYCPP_MIME_BUILD_EXAMPLES_DEFAULT ON)
endif()
option(POLYCPP_MIME_BUILD_EXAMPLES "Build examples" ${POLYCPP_MIME_BUILD_EXAMPLES_DEFAULT})

if(POLYCPP_MIME_BUILD_EXAMPLES)
    file(GLOB POLYCPP_MIME_EXAMPLES CONFIGURE_DEPENDS examples/*.cpp)
    foreach(example_source IN LISTS POLYCPP_MIME_EXAMPLES)
        get_filename_component(example_name "${example_source}" NAME_WE)
        set(example_target "polycpp_mime_example_${example_name}")
        add_executable("${example_target}" "${example_source}")
        target_link_libraries("${example_target}" PRIVATE polycpp::mime)
        set_target_properties("${example_target}" PROPERTIES
            OUTPUT_NAME "${example_name}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
        )
    endforeach()
endif()
