find_package(GTest REQUIRED)

add_executable(e2e_tests testcases.cpp)
target_link_libraries(e2e_tests PRIVATE loop_ssa_lib GTest::gtest_main GTest::gtest)

include(GoogleTest)
gtest_discover_tests(e2e_tests)