cmake_minimum_required(VERSION 3.5)
project(RapidYaml)

set(RYML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(RYML_SRC
    ${RYML_SRC}/ryml/ryml.hpp)

option(RYML_BUILD_LIBYAML "" ON)
if(RYML_BUILD_LIBYAML)
    add_subdirectory(libyaml libyaml)
endif()

option(RYML_BUILD_TESTS "Build RapidYaml tests" ON)
if(RYML_BUILD_TESTS)
    add_subdirectory(test test)
endif()
