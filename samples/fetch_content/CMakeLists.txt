cmake_minimum_required(VERSION 3.12)
project(ryml-sample-quick_tour-fetch_content LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
    ryml-fetch_content
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG samples
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(ryml-fetch_content)

set(exe ryml-sample-quick_tour-fetch_content)

add_executable(${exe} ../quick_tour.cpp)
target_link_libraries(${exe} ryml)

add_custom_target(run ${exe}
    COMMAND $<TARGET_FILE:${exe}>
    DEPENDS ${exe}
    COMMENT "running: $<TARGET_FILE:${exe}>")
