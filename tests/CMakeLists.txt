
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(fast_division_tests fast_division_tests.hpp
                    fast_division_tests.cpp
                    test_harness.cpp)

set_target_properties(fast_division_tests PROPERTIES FOLDER "Fast_Division_Tests")