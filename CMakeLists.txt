cmake_minimum_required(VERSION 3.6.0)

project("Fast_Division")

option(FAST_DIVISION_BUILD_EXAMPLE "Build fast division example application?" OFF)
option(FAST_DIVISION_BUILD_TESTS "Build fast division tests?" OFF)

include_directories("include")

# Since this is a header-only library we can create an interface library.
add_library(fast_division INTERFACE)

set(FAST_DIVISION_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/fast_division)

set(FAST_DIVISION_SOURCES 
    ${FAST_DIVISION_SOURCE_DIR}/utility/log2i.hpp
    ${FAST_DIVISION_SOURCE_DIR}/utility/high_multiplication.hpp
    ${FAST_DIVISION_SOURCE_DIR}/utility/associated_types.hpp
    ${FAST_DIVISION_SOURCE_DIR}/fast_division.hpp)
target_include_directories(fast_division INTERFACE include)
target_sources(fast_division INTERFACE ${FAST_DIVISION_SOURCES})
source_group(fast_division FILES ${FAST_DIVISION_SOURCES})

# Hack-ish way to add the library to IDEs as a project.
add_custom_target(fast_division_library SOURCES ${FAST_DIVISION_SOURCES})

if(FAST_DIVISION_BUILD_EXAMPLE)
    add_subdirectory("example")
endif()

if(FAST_DIVISION_BUILD_TESTS)
    add_subdirectory("tests")
endif()

