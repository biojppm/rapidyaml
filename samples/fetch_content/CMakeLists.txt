cmake_minimum_required(VERSION 3.12)
project(ryml-sample-importing-fetch_content LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
    ryml-fetch_content
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG samples
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(ryml-fetch_content)

set(exe ryml-consumer-fetch_content)

add_executable(${exe} main.cpp)
target_link_libraries(${exe} ryml)
add_custom_command(TARGET ${exe} POST_BUILD
    COMMAND $<TARGET_FILE:${exe}>
    COMMENT "running test: $<TARGET_FILE:${exe}>")
