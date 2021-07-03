cmake_minimum_required(VERSION 3.12)
project(ryml-sample-quick_tour-add_subdirectory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../../ ryml-add_subdirectory)

set(exe ryml-sample-quick_tour-add_subdirectory)

add_executable(${exe} ../quick_tour.cpp)
target_link_libraries(${exe} ryml)

add_custom_target(run ${exe}
    COMMAND $<TARGET_FILE:${exe}>
    DEPENDS ${exe}
    COMMENT "running: $<TARGET_FILE:${exe}>")
