cmake_minimum_required(VERSION 3.9) # because of the GoogleTest module
project(ryml)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(c4Project)

c4_declare_project(ryml)

option(RYML_DEFAULT_CALLBACKS "Enable ryml default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable ryml debug code. Very verbose." OFF)#${RYML_DEV})


#-------------------------------------------------------
set(RYML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

set(RYML_CPP
    ${RYML_SRC_DIR}/c4/yml/common.cpp
    ${RYML_SRC_DIR}/c4/yml/parse.cpp
    ${RYML_SRC_DIR}/c4/yml/tree.cpp
    )

set(RYML_HPP
    ${RYML_SRC_DIR}/ryml.hpp
    ${RYML_SRC_DIR}/ryml_std.hpp
    ${RYML_SRC_DIR}/c4/yml/detail/stack.hpp
    ${RYML_SRC_DIR}/c4/yml/detail/parser_dbg.hpp
    ${RYML_SRC_DIR}/c4/yml/common.hpp
    ${RYML_SRC_DIR}/c4/yml/emit.def.hpp
    ${RYML_SRC_DIR}/c4/yml/emit.hpp
    ${RYML_SRC_DIR}/c4/yml/node.hpp
    ${RYML_SRC_DIR}/c4/yml/parse.hpp
    ${RYML_SRC_DIR}/c4/yml/std/map.hpp
    ${RYML_SRC_DIR}/c4/yml/std/std.hpp
    ${RYML_SRC_DIR}/c4/yml/std/string.hpp
    ${RYML_SRC_DIR}/c4/yml/std/vector.hpp
    ${RYML_SRC_DIR}/c4/yml/subs.hpp
    ${RYML_SRC_DIR}/c4/yml/to_str.hpp
    ${RYML_SRC_DIR}/c4/yml/tree.hpp
    ${RYML_SRC_DIR}/c4/yml/writer.hpp
    ${RYML_SRC_DIR}/c4/yml/yml.hpp
    )

set(RYML_SRC ${RYML_CPP} ${RYML_HPP} ${CMAKE_CURRENT_SOURCE_DIR}/ryml.natvis)

c4_add_target(RYML ryml LIBRARY SANITIZE
    SOURCES ${RYML_SRC}
    INC_DIRS ${RYML_SRC_DIR})

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml PRIVATE RYML_NO_DEFAULT_CALLBACKS)
endif()
if(RYML_DBG)
    target_compile_definitions(ryml PRIVATE RYML_DBG)
endif()

#-------------------------------------------------------
if(RYML_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test test)
endif()

#-------------------------------------------------------
if(RYML_BUILD_BENCHMARKS)
    #
endif()
