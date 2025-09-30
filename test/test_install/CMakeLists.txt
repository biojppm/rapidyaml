cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ryml
    DESCRIPTION "Test project for an installed ryml"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(../../ext/c4core/cmake/c4Project.cmake)
c4_project(VERSION 0.10.0
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")


if(RYML_TEST_INSTALL_PACKAGE_MODE)
    find_package(ryml REQUIRED)
    add_library(ryml ALIAS ryml::ryml)
else()
    find_path(RYML_INCLUDE_DIR ryml.hpp PATH_SUFFIXES include REQUIRED)
    find_library(RYML_LIBRARY ryml REQUIRED)
    add_library(ryml UNKNOWN IMPORTED)
    set_target_properties(ryml PROPERTIES IMPORTED_LOCATION "${RYML_LIBRARY}")
    target_include_directories(ryml INTERFACE "${RYML_INCLUDE_DIR}")
endif()

enable_testing()
set(RYML_INSTALL_TEST ON)
include(../../ext/testbm.cmake)
add_subdirectory(../../test test)
