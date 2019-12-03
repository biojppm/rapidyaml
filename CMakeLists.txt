cmake_minimum_required(VERSION 3.13)
project(ryml)

include(./extern/c4core/cmake/c4Project.cmake)

c4_declare_project(RYML STANDALONE
    DESC "Rapid YAML parsing and emitting"
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>"
    MAJOR 0 MINOR 1 RELEASE 0)

#-------------------------------------------------------
#
option(RYML_DEFAULT_CALLBACKS "Enable ryml's default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable (very verbose) ryml debug prints." OFF)
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)

#-------------------------------------------------------
set(RYML_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(RYML_EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/extern)
set(RYML_API_DIR ${CMAKE_CURRENT_LIST_DIR}/api)

#-------------------------------------------------------

set(RYML_CPP
    c4/yml/common.cpp
    c4/yml/parse.cpp
    c4/yml/tree.cpp
)

set(RYML_HPP
    ryml.hpp
    ryml_std.hpp
    c4/yml/detail/stack.hpp
    c4/yml/detail/parser_dbg.hpp
    c4/yml/common.hpp
    c4/yml/emit.def.hpp
    c4/yml/emit.hpp
    c4/yml/node.hpp
    c4/yml/parse.hpp
    c4/yml/std/map.hpp
    c4/yml/std/std.hpp
    c4/yml/std/string.hpp
    c4/yml/std/vector.hpp
    c4/yml/tree.hpp
    c4/yml/writer.hpp
    c4/yml/yml.hpp
    )

set (C4_HPP
  c4/allocator.hpp
  c4/base64.hpp
  c4/bitmask.hpp
  c4/blob.hpp
  c4/c4_pop.hpp
  c4/c4_push.hpp
  c4/charconv.hpp
  c4/char_traits.hpp
  c4/common.hpp
  c4/compiler.hpp
  c4/config.hpp
  c4/cpu.hpp
  c4/ctor_dtor.hpp
  c4/enum.hpp
  c4/error.hpp
  c4/format.hpp
  c4/hash.hpp
  c4/language.hpp
  c4/memory_resource.hpp
  c4/memory_util.hpp
  c4/platform.hpp
  c4/preprocessor.hpp
  c4/restrict.hpp
  c4/span.hpp
  c4/substr.hpp
  c4/szconv.hpp
  c4/time.hpp
  c4/type_name.hpp
  c4/types.hpp
  c4/unrestrict.hpp
  c4/windows.hpp
  c4/windows_pop.hpp
  c4/windows_push.hpp
  c4/std/std.hpp
  c4/std/string.hpp
  c4/std/tuple.hpp
  c4/std/vector.hpp
)

set(RYML_FILES
    ../ryml.natvis
)

set(RYML_SRC ${RYML_CPP} ${RYML_HPP} ${RYML_FILES}
  SOURCE_ROOT ${RYML_SRC_DIR})

#-------------------------------------------------------

ryml_require_subproject(c4core
  SUBDIRECTORY ${RYML_EXT_DIR}/c4core)

if(RYML_STANDALONE)
    # use c4core-interface as PRIVATE to prevent sources from propagating
    ryml_add_library(ryml SANITIZE
        SOURCES ${RYML_SRC}
        PUBLIC_INC_DIRS ${RYML_SRC_DIR} ${RYML_EXT_DIR}/c4core/src ${RYML_EXT_DIR}/c4core/extern
        PRIVATE_LIBS c4core-interface
    )
else()
    ryml_add_library(ryml SANITIZE
        SOURCES ${RYML_SRC}
        PUBLIC_INC_DIRS ${RYML_SRC_DIR}
        PUBLIC_LIBS c4core
    )
endif(RYML_STANDALONE)

ryml_add_library(ryml-interface LIBRARY_TYPE INTERFACE
    SOURCES ${RYML_SRC}
    INTERFACE_INC_DIRS ${RYML_SRC_DIR}
    INTERFACE_LIBS c4core-interface
    )

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml           PRIVATE   RYML_NO_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml-interface INTERFACE RYML_NO_DEFAULT_CALLBACKS)
endif()

if(RYML_DBG)
    target_compile_definitions(ryml           PRIVATE   RYML_DBG)
    target_compile_definitions(ryml-interface INTERFACE RYML_DBG)
endif()

#-------------------------------------------------------
# extern libraries, used only for testing/benchmarking
if(RYML_BUILD_TESTS OR RYML_BUILD_BENCHMARKS)
    set(ed ${CMAKE_CURRENT_BINARY_DIR}/extern) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)
    #
    # these are used both for testing and benchmarking
    #
    ryml_require_module(c4fs
        REMOTE
        GIT_REPOSITORY https://github.com/biojppm/c4fs
        GIT_TAG master)
    #
    c4_override(BUILD_TESTING OFF)
    ryml_require_module(libyaml REMOTE
        GIT_REPOSITORY https://github.com/yaml/libyaml
        GIT_TAG master)
    c4_set_folder_remote_project_targets(ext yaml)
    #
    c4_override(YAML_CPP_BUILD_TESTS OFF)
    c4_override(YAML_CPP_BUILD_TOOLS OFF)
    ryml_import_remote_proj(yaml-cpp ${ed}/yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG master)
    c4_set_folder_remote_project_targets(ext yaml-cpp)
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

#-------------------------------------------------------

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE rapidyaml)

set(CONF_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/ryml/${HDRDIR})
set(CONF_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib/ryml)
set(CONF_LIBRARIES    ryml)
configure_file(rapidyamlConfig.cmake.in
  "${PROJECT_BINARY_DIR}/rapidyamlConfig.cmake" @ONLY)

install(FILES "${PROJECT_BINARY_DIR}/rapidyamlConfig.cmake" DESTINATION share/ryml)
install(TARGETS ryml DESTINATION lib/ryml)
install(TARGETS c4core DESTINATION lib/ryml)
foreach(HDR ${RYML_HPP})
  get_filename_component(HDRDIR ${HDR} DIRECTORY)
  install(FILES ${RYML_SRC_DIR}/${HDR} DESTINATION include/ryml/${HDRDIR})
endforeach()
foreach(HDR ${C4_HPP})
  get_filename_component(HDRDIR ${HDR} DIRECTORY)
  install(FILES ${RYML_EXT_DIR}/c4core/src/${HDR} DESTINATION include/ryml/${HDRDIR})
endforeach()
