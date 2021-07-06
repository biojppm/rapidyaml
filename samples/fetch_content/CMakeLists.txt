cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG master     # or a particular branch, tag or commit
    GIT_SHALLOW FALSE  # ensure submodules are checked out
)
FetchContent_MakeAvailable(ryml)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml)

add_custom_target(run ryml-quickstart
    COMMAND $<TARGET_FILE:ryml-quickstart>
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart>")
