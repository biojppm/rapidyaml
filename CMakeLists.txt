cmake_minimum_required(VERSION 3.9) # because of the GoogleTest module
project(ryml)

include(./extern/c4core/cmake/c4Project.cmake)

c4_declare_project(ryml STANDALONE
    DESC "Rapid YAML parsing and emitting"
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>"
    MAJOR 0 MINOR 1 RELEASE 0)

#-------------------------------------------------------
#
option(RYML_DEFAULT_CALLBACKS "Enable ryml default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable (very verbose) ryml debug prints." OFF)
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)

#-------------------------------------------------------
set(RYML_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/extern)
set(RYML_API_DIR ${CMAKE_CURRENT_LIST_DIR}/api)

#-------------------------------------------------------

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
    ${RYML_SRC_DIR}/c4/yml/tree.hpp
    ${RYML_SRC_DIR}/c4/yml/writer.hpp
    ${RYML_SRC_DIR}/c4/yml/yml.hpp
    )

set(RYML_FILES
    ${CMAKE_CURRENT_LIST_DIR}/ryml.natvis
    )

set(RYML_SRC
    ${RYML_CPP}
    ${RYML_HPP}
    ${RYML_FILES})

#-------------------------------------------------------

c4_require_module(ryml c4core
    SUBDIRECTORY ${RYML_EXT_DIR}/c4core)

c4_add_library(RYML ryml SANITIZE
    SOURCES ${RYML_SRC}
    INC_DIRS ${RYML_SRC_DIR}
    LIBS c4core
    )

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml PRIVATE RYML_NO_DEFAULT_CALLBACKS)
endif()

if(RYML_DBG)
    target_compile_definitions(ryml PRIVATE RYML_DBG)
endif()

#-------------------------------------------------------
# extern libraries, used only for testing/benchmarking
if(RYML_BUILD_TESTS OR RYML_BUILD_BENCHMARKS)
    set(ed ${CMAKE_CURRENT_BINARY_DIR}/extern) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)
    #
    # these are used both for testing and benchmarking
    #
    c4_import_remote_proj(ryml c4fs ${ed}/c4fs
        GIT_REPOSITORY https://github.com/biojppm/c4fs
        GIT_TAG master)
    #
    c4_override(BUILD_TESTING OFF)
    c4_import_remote_proj(ryml libyaml ${ed}/libyaml
        GIT_REPOSITORY https://github.com/yaml/libyaml
        GIT_TAG master)
    #
    c4_override(YAML_CPP_BUILD_TESTS OFF)
    c4_override(YAML_CPP_BUILD_TOOLS OFF)
    c4_import_remote_proj(ryml yaml-cpp ${ed}/yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG master)
    set(ryml_yaml_cpp_inc ${ed}/yaml-cpp/src/include)
endif()

#-------------------------------------------------------
if(RYML_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test test)
endif()

#-------------------------------------------------------
if(RYML_BUILD_BENCHMARKS)
    add_subdirectory(bm bm)
endif()

#-------------------------------------------------------
if(RYML_BUILD_API)
    add_subdirectory(api api)
endif()
