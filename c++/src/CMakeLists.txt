add_library(
        planarity_lib
        graph.cpp
        demoucron.cpp
)

target_include_directories(
        planarity_lib PUBLIC
        ${PLANARITY_INCLUDE_DIR}
)

target_include_directories(
        planarity_lib PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)