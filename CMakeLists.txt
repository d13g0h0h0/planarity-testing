cmake_minimum_required(VERSION 4.1)
project(planarity_testing)

set(CMAKE_CXX_STANDARD 23)

add_executable(planarity_testing
        examples/c++/main.cpp)

target_include_directories(planarity_testing PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/demoucron/c++
        ${CMAKE_SOURCE_DIR}/definitions/c++
)
