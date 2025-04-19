cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart
    DESCRIPTION "Shows how to use a custom c4core version,
                 different from ryml's c4core submodule"
    LANGUAGES CXX)

set(C4CORE_REPO https://github.com/biojppm/c4core CACHE STRING "")
set(C4CORE_TAG master CACHE STRING "")
include(FetchContent)
message(STATUS "fetch c4core from repo: ${C4CORE_REPO}")
message(STATUS "fetch c4core from tag: ${C4CORE_TAG}")
FetchContent_Declare(c4core
    GIT_REPOSITORY ${C4CORE_REPO}
    GIT_TAG ${C4CORE_TAG}
    GIT_SHALLOW FALSE  # ensure submodules are checked out
)
FetchContent_MakeAvailable(c4core)

# and now use RYML_STANDALONE=OFF to inform ryml to link with c4core
# (which must already be available as a target), instead of using the
# c4core sources packaged with ryml:
set(RYML_STANDALONE OFF CACHE BOOL "" FORCE) # override the default in ryml
add_subdirectory(../.. ryml)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml::ryml)

add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart>
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart>")
