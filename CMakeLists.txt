cmake_minimum_required(VERSION 3.9) # because of the GoogleTest module
project(ryml)

include(./extern/c4core/cmake/c4Project.cmake)

c4_declare_project(ryml)

set(RYML_VERSION 0.1.0)
set(RYML_DESC "Rapid YAML parsing and emitting")
set(RYML_AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

#-------------------------------------------------------
option(RYML_STANDALONE "" ON)
option(RYML_DEFAULT_CALLBACKS "Enable ryml default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable ryml debug code. Very verbose." OFF)#${RYML_DEV})
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)

#-------------------------------------------------------
set(RYML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
set(RYML_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/api)

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

set(RYML_SRC ${RYML_CPP} ${RYML_HPP} ${CMAKE_CURRENT_SOURCE_DIR}/ryml.natvis)

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
    #
    set(ryml_ext_dir ${CMAKE_CURRENT_BINARY_DIR}/extern)
    macro(_override_option opt val)
        set(${opt} ${val} CACHE BOOL "" FORCE)
    endmacro()
    #
    c4_import_remote_proj(ryml c4fs ${ryml_ext_dir}/c4fs
        GIT_REPOSITORY https://github.com/biojppm/c4fs
        GIT_TAG master)
    #
    _override_option(BUILD_TESTING OFF)
    c4_import_remote_proj(ryml libyaml ${ryml_ext_dir}/libyaml
        GIT_REPOSITORY https://github.com/yaml/libyaml
        GIT_TAG master)
    #
    _override_option(YAML_CPP_BUILD_TESTS OFF)
    _override_option(YAML_CPP_BUILD_TOOLS OFF)
    c4_import_remote_proj(ryml yaml-cpp ${ryml_ext_dir}/yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG master)
    #
    _override_option(JSONCPP_WITH_TESTS OFF)
    _override_option(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
    _override_option(JSONCPP_WITH_WARNING_AS_ERROR OFF)
    _override_option(JSONCPP_WITH_STRICT_ISO OFF)
    _override_option(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
    _override_option(JSONCPP_WITH_CMAKE_PACKAGE OFF)
    c4_import_remote_proj(ryml jsoncpp ${ryml_ext_dir}/jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
        GIT_TAG master)
    #
    _override_option(JSON_BuildTests OFF)
    _override_option(JSON_Install OFF)
    _override_option(JSON_MultipleHeaders OFF)
    c4_import_remote_proj(ryml nlohmann_json ${ryml_ext_dir}/nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG master)
    #
    c4_download_remote_proj(ryml rapidjson ${ryml_ext_dir}/rapidjson
        GIT_REPOSITORY https://github.com/Tencent/rapidjson
        GIT_TAG version1.1.0)
    set(RYML_RAPIDJSON_INC_DIR ${ryml_ext_dir}/rapidjson/src/include)
    #
    c4_download_remote_proj(ryml sajson ${ryml_ext_dir}/sajson
        GIT_REPOSITORY https://github.com/chadaustin/sajson
        GIT_TAG master)
    set(RYML_SAJSON_INC_DIR ${ryml_ext_dir}/sajson/src/include)
    #
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
