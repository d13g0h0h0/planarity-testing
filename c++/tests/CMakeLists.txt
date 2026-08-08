add_executable(unit_tests
        hello_test.cc
        demoucron_test.cpp
)

target_link_libraries(unit_tests PRIVATE
        planarity_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)