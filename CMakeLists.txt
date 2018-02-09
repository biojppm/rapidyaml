cmake_minimum_required(VERSION 3.9) # because of the GoogleTest module
project(ryml)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(c4project)

option(RYML_DEV "enable ryml development targets: tests, benchmarks, sanitize, static analysis, coverage" OFF)
option(RYML_BUILD_TESTS "build ryml unit tests" ${RYML_DEV})
option(RYML_BUILD_BENCHMARKS "build ryml benchmarks" ${RYML_DEV})

option(RYML_LINT_PVS_STUDIO OFF)
c4_setup_sanitize(RYML ${RYML_DEV})
c4_setup_static_analysis(RYML ${RYML_DEV})
c4_setup_valgrind(RYML ${RYML_DEV})
c4_setup_coverage(RYML ${RYML_DEV})

#-------------------------------------------------------
set(RYML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

set(RYML_CPP
    ${RYML_SRC_DIR}/c4/yml/parse.cpp
    ${RYML_SRC_DIR}/c4/yml/tree.cpp
    )

set(RYML_HPP
    ${RYML_SRC_DIR}/c4/yml/detail/stack.hpp
    ${RYML_SRC_DIR}/c4/yml/common.hpp
    ${RYML_SRC_DIR}/c4/yml/emit.def.hpp
    ${RYML_SRC_DIR}/c4/yml/emit.hpp
    ${RYML_SRC_DIR}/c4/yml/node.hpp
    ${RYML_SRC_DIR}/c4/yml/parse.hpp
    ${RYML_SRC_DIR}/c4/yml/span.hpp
    ${RYML_SRC_DIR}/c4/yml/to_str.hpp
    ${RYML_SRC_DIR}/c4/yml/tree.hpp
    ${RYML_SRC_DIR}/c4/yml/writer.hpp
    ${RYML_SRC_DIR}/c4/yml/yml.hpp
    )

set(RYML_SRC ${RYML_CPP} ${RYML_HPP})

c4_add_target(RYML ryml LIBRARY SANITIZE
    SOURCES ${RYML_SRC}
    INC_DIRS ${RYML_SRC_DIR})

#-------------------------------------------------------
if(RYML_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test test)
endif()

#-------------------------------------------------------
if(RYML_BUILD_BENCHMARKS)
    #
endif()
