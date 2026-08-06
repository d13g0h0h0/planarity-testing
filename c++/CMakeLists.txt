cmake_minimum_required(VERSION 4.1)
project(planarity_testing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
        planarity_testing
        main.cpp
)

target_include_directories(
        planarity_testing PRIVATE
        ${CMAKE_SOURCE_DIR}
)

enable_testing()

add_executable(
        hello_test
        hello_test.cc
)

target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)

