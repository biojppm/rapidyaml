cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(ryml-quickstart LANGUAGES CXX)

set(RYML_REPO_URL https://github.com/biojppm/rapidyaml CACHE STRING "")
set(RYML_BRANCH_NAME master CACHE STRING "")
message(STATUS "FetchContent from repo: ${RYML_REPO_URL}")
message(STATUS "FetchContent from branch: ${RYML_BRANCH_NAME}")
include(FetchContent)
FetchContent_Declare(ryml
    GIT_REPOSITORY ${RYML_REPO_URL}
    GIT_TAG ${RYML_BRANCH_NAME}
    GIT_SHALLOW FALSE  # ensure submodules are checked out
)
FetchContent_MakeAvailable(ryml)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml::ryml)

add_custom_target(run ryml-quickstart
    COMMAND $<TARGET_FILE:ryml-quickstart>
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart>")
