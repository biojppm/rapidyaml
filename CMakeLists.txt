cmake_minimum_required(VERSION 3.9)
project(ryml)

include(./ext/c4core/cmake/c4Project.cmake)

c4_declare_project(ryml
    STANDALONE
    DESC "Rapid YAML parsing and emitting"
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>"
    MAJOR 0 MINOR 1 RELEASE 0)


#-------------------------------------------------------

option(RYML_DEFAULT_CALLBACKS "Enable ryml's default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable (very verbose) ryml debug prints." OFF)
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)


#-------------------------------------------------------

c4_require_subproject(c4core INCORPORATE
    SUBDIRECTORY ${RYML_EXT_DIR}/c4core)

c4_add_library(ryml
    SOURCES
        ryml.hpp
        ryml_std.hpp
        c4/yml/detail/stack.hpp
        c4/yml/detail/parser_dbg.hpp
        c4/yml/common.hpp
        c4/yml/common.cpp
        c4/yml/emit.def.hpp
        c4/yml/emit.hpp
        c4/yml/export.hpp
        c4/yml/node.hpp
        c4/yml/parse.hpp
        c4/yml/parse.cpp
        c4/yml/preprocess.hpp
        c4/yml/preprocess.cpp
        c4/yml/std/map.hpp
        c4/yml/std/std.hpp
        c4/yml/std/string.hpp
        c4/yml/std/vector.hpp
        c4/yml/tree.hpp
        c4/yml/tree.cpp
        c4/yml/writer.hpp
        c4/yml/yml.hpp
        ryml.natvis
    SOURCE_ROOT ${RYML_SRC_DIR}
    INC_DIRS
        $<BUILD_INTERFACE:${RYML_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
    LIBS c4core
    INCORPORATE c4core
    )

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml PRIVATE RYML_NO_DEFAULT_CALLBACKS)
endif()

if(RYML_DBG)
    target_compile_definitions(ryml PRIVATE RYML_DBG)
endif()


#-------------------------------------------------------

c4_install_target(ryml)
c4_install_exports(DEPENDENCIES c4core)


#-------------------------------------------------------
# developer targets

# extern libraries, used only for testing/benchmarking
if(RYML_BUILD_TESTS OR RYML_BUILD_BENCHMARKS)
    set(ed ${CMAKE_CURRENT_BINARY_DIR}/subprojects) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)
    #
    # these are used both for testing and benchmarking
    #
    c4_require_subproject(c4fs
        REMOTE
        GIT_REPOSITORY https://github.com/biojppm/c4fs
        GIT_TAG master)
    #
    c4_override(BUILD_TESTING OFF)
    c4_require_subproject(libyaml REMOTE
        GIT_REPOSITORY https://github.com/yaml/libyaml
        GIT_TAG master)
    c4_set_folder_remote_project_targets(ext yaml)
    #
    c4_override(YAML_CPP_BUILD_TESTS OFF)
    c4_override(YAML_CPP_BUILD_TOOLS OFF)
    c4_override(YAML_CPP_BUILD_CONTRIB OFF)
    c4_override(YAML_CPP_BUILD_INSTALL OFF)
    c4_import_remote_proj(yaml-cpp ${ed}/yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        # the master branch regularly screws up on windows builds.
        # so use fixed pre-validated commit hashes
        GIT_TAG 587b24e2eedea1afa21d79419008ca5f7bda3bf4)
    c4_set_folder_remote_project_targets(ext/yaml-cpp yaml-cpp format)
    set(ryml_yaml_cpp_inc ${ed}/yaml-cpp/src/include)
endif()

c4_add_dev_targets()
