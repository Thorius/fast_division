# Initialize CMake's testing capabilities
enable_testing()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(FAST_DIVISION_TEST_SOURCES  fast_division_tests.hpp
                                fast_division_tests.cpp
                                test_harness.cpp)
add_executable(fast_division_tests ${FAST_DIVISION_TEST_SOURCES})
target_link_libraries(fast_division_tests INTERFACE fast_division )
add_test(fast_division_tests fast_division_tests)

set_target_properties(fast_division_tests PROPERTIES FOLDER "Fast_Division_Tests")