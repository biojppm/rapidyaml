cmake_minimum_required(VERSION 3.12)

include(../../ext/c4core/cmake/c4Project.cmake)
project(ryml-quickstart
    DESCRIPTION "Shows how to use a custom c4core version,
                 different from ryml's c4core submodule"
    LANGUAGES CXX)
c4_project(VERSION 0.1.0 STANDALONE
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

# download a different c4core version from the repo
c4_require_subproject(c4core REMOTE
    GIT_REPOSITORY https://github.com/biojppm/c4core.git
    GIT_TAG master)
# and now, within ryml's CMakeLists.txt, the call to
# c4_require_subproject(c4core) will use the version
# downloaded above, instead of the submodule version from
# the ryml repo:
c4_require_subproject(ryml SUBDIRECTORY ../..)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml)

add_custom_target(run ryml-quickstart
    COMMAND $<TARGET_FILE:ryml-quickstart>
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart>")
