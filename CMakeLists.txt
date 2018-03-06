cmake_minimum_required(VERSION 3.9) # because of the GoogleTest module
project(ryml)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(c4project)

option(RYML_DEV "enable ryml development targets: tests, benchmarks, sanitizers, static analysis, coverage" OFF)
option(RYML_BUILD_TESTS "build ryml unit tests" ${RYML_DEV})
option(RYML_BUILD_BENCHMARKS "build ryml benchmarks" ${RYML_DEV})

option(RYML_LINT_PVS_STUDIO OFF)
option(RYML_VALGRIND_SGCHECK OFF) # crashing in ubuntu16.04
if(RYML_BUILD_TESTS)
    enable_testing()
endif()
c4_setup_testing(RYML ${RYML_BUILD_TESTS})
c4_setup_sanitize(RYML ${RYML_DEV})
c4_setup_static_analysis(RYML ${RYML_DEV})
c4_setup_valgrind(RYML ${RYML_DEV})
c4_setup_coverage(RYML ${RYML_DEV})


#-------------------------------------------------------
option(RYML_DEFAULT_CALLBACKS "Enable ryml default implementation of callbacks: allocate(), free(), error()" ON)
option(RYML_DBG "Enable ryml debug code. Very verbose." OFF)#${RYML_DEV})

option(RYML_PEDANTIC "Compile in pedantic mode" ${RYML_DEV})
set(RYML_CXX_FLAGS "")
if(RYML_PEDANTIC)
    if(MSVC)
        set(RYML_CXX_FLAGS "${RYML_CXX_FLAGS} /W4")
    else()
        set(RYML_CXX_FLAGS "${RYML_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic -Wfloat-equal -fstrict-aliasing")
    endif()
endif()

option(RYML_WERROR "Compile with warnings as errors" ${RYML_DEV})
if(RYML_WERROR)
    if(MSVC)
        set(RYML_CXX_FLAGS "${RYML_CXX_FLAGS} /WX")
        #set(RYML_LINKER_FLAGS "${RYML_LINKER_FLAGS} /WX")
    else()
        set(RYML_CXX_FLAGS "${RYML_CXX_FLAGS} -Werror -pedantic-errors")
    endif()
endif()


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
    static_analysis_add_tests(RYML ryml)
endif()

#-------------------------------------------------------
if(RYML_BUILD_BENCHMARKS)
    #
endif()
