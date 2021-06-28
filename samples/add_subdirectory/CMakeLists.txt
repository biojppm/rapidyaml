cmake_minimum_required(VERSION 3.12)
project(ryml-consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../../ ryml-add_subdirectory)

# run some checks (used in CI tests) to ensure
# using ryml as a subdirectory does not interfere
# with the existing settings
include(./check.cmake)

set(exe ryml-consumer-add_subdirectory)

add_executable(${exe} main.cpp)
target_link_libraries(${exe} ryml)
add_custom_command(TARGET ${exe} POST_BUILD
    COMMAND $<TARGET_FILE:${exe}>
    COMMENT "running test: $<TARGET_FILE:${exe}>")
