cmake_minimum_required(VERSION 3.9)
project(ryml)

set(RYML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

set(RYML_SRC
    ${RYML_SRC_DIR}/ryml/common.hpp
    ${RYML_SRC_DIR}/ryml/ryml.hpp
    #${RYML_SRC_DIR}/ryml/ryml.cpp # for now this is #included in ryml.hpp ; need to setup the library plumbing
    ${RYML_SRC_DIR}/ryml/span.hpp
    ${RYML_SRC_DIR}/ryml/stack.hpp)

option(RYML_BUILD_TESTS "Build ryml tests" ON)
if(RYML_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test test)
endif()
