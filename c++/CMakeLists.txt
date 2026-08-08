cmake_minimum_required(VERSION 4.1)
project(planarity_testing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# taken from Google Test's quickstart guide
include(FetchContent)
FetchContent_Declare(
        googletest
        # might have to update the source
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(PLANARITY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)

