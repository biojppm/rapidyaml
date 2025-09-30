cmake_minimum_required(VERSION 3.12)
include(./ext/c4core/cmake/c4Project.cmake)
project(ryml
    DESCRIPTION "Rapid YAML parsing and emitting"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(./compat.cmake)

c4_project(VERSION 0.10.0 STANDALONE
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")


#-------------------------------------------------------

option(RYML_WITH_TAB_TOKENS "Enable parsing of tabs after ':' and '-'. This is costly and disabled by default." OFF)
option(RYML_DEFAULT_CALLBACKS "Enable ryml's default implementation of callbacks: allocate(), free(), error()" ON)
if(RYML_DEFAULT_CALLBACKS)
    option(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS "Throw exceptions instead of calling abort in the default error handler provided by ryml" OFF)
endif()
option(RYML_USE_ASSERT "Enable assertions regardless of build type. Default is only when NDEBUG is not defined (which is in release builds). This causes a slowdown of the code." OFF)
option(RYML_BUILD_TOOLS "Build tools" OFF)
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)
option(RYML_DBG "Enable (very verbose) debug prints." OFF)

option(RYML_INSTALL "Enable install target" ON)


#-------------------------------------------------------

c4_require_subproject(c4core INCORPORATE
    SUBDIRECTORY ${RYML_EXT_DIR}/c4core
    OVERRIDE C4CORE_INSTALL ${RYML_INSTALL}
)

c4_add_library(ryml
    SOURCES
        ryml.hpp
        ryml_std.hpp
        c4/yml/detail/checks.hpp
        c4/yml/detail/dbgprint.hpp
        c4/yml/detail/print.hpp
        c4/yml/detail/stack.hpp
        c4/yml/common.hpp
        c4/yml/common.cpp
        c4/yml/emit.def.hpp
        c4/yml/emit.hpp
        c4/yml/event_handler_stack.hpp
        c4/yml/event_handler_tree.hpp
        c4/yml/filter_processor.hpp
        c4/yml/fwd.hpp
        c4/yml/export.hpp
        c4/yml/node.hpp
        c4/yml/node.cpp
        c4/yml/node_type.hpp
        c4/yml/node_type.cpp
        c4/yml/parser_state.hpp
        c4/yml/parse.hpp
        c4/yml/parse.cpp
        c4/yml/parse_engine.hpp
        c4/yml/parse_engine.def.hpp
        c4/yml/preprocess.hpp
        c4/yml/preprocess.cpp
        c4/yml/reference_resolver.hpp
        c4/yml/reference_resolver.cpp
        c4/yml/std/map.hpp
        c4/yml/std/std.hpp
        c4/yml/std/string.hpp
        c4/yml/std/vector.hpp
        c4/yml/tag.hpp
        c4/yml/tag.cpp
        c4/yml/tree.hpp
        c4/yml/tree.cpp
        c4/yml/version.hpp
        c4/yml/version.cpp
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

if(RYML_WITH_TAB_TOKENS)
    target_compile_definitions(ryml PUBLIC RYML_WITH_TAB_TOKENS)
endif()

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml PRIVATE RYML_NO_DEFAULT_CALLBACKS)
else()
    if(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS)
        target_compile_definitions(ryml PRIVATE RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS)
    endif()
endif()

if(RYML_DBG)
    target_compile_definitions(ryml PRIVATE RYML_DBG)
endif()

if(RYML_USE_ASSERT)
    target_compile_definitions(ryml PUBLIC RYML_USE_ASSERT=1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    option(RYML_FANALYZER "Compile with -fanalyzer https://gcc.gnu.org/onlinedocs/gcc-13.2.0/gcc/Static-Analyzer-Options.html" OFF)
    if(RYML_FANALYZER)
        target_compile_options(ryml PUBLIC -fanalyzer)
    endif()
endif()


#-------------------------------------------------------

if(RYML_INSTALL)
    c4_install_target(ryml)
    c4_install_exports(DEPENDENCIES c4core)
    c4_pack_project()
endif()


#-------------------------------------------------------
# developer targets

# extern libraries, used only for testing/benchmarking
if(RYML_BUILD_TESTS OR RYML_BUILD_BENCHMARKS OR RYML_BUILD_TOOLS)
    include(ext/testbm.cmake)
endif()

if(RYML_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

c4_add_dev_targets()

add_custom_target(ryml-uninstall
    "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/cmake/uninstall.cmake"
)


#-------------------------------------------------------
# clang-tidy

function(ryml_setup_clang_tidy rootdir)
    get_target_property(srcs ryml SOURCES)
    string(REPLACE "${rootdir}/" "./" srcs "${srcs}")
    set(exclude ./ext/.* parse_engine.def.hpp .natvis)
    foreach(e ${exclude})
        list(FILTER srcs EXCLUDE REGEX ${e})
    endforeach()
    set(cmd ${srcs} -p ${CMAKE_BINARY_DIR}
            "--config-file=${rootdir}/.clang-tidy"
            "--header-filter=${rootdir}/src/.*")
    add_custom_target(ryml-clang-tidy
        COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMAND ${CLANG_TIDY} --version
        COMMAND ${CLANG_TIDY} ${cmd} --dump-config
        COMMAND ${CLANG_TIDY} ${cmd} --list-checks
        COMMAND ${CLANG_TIDY} ${cmd}
        WORKING_DIRECTORY ${rootdir}
        VERBATIM
    )
endfunction()

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    ryml_setup_clang_tidy(${CMAKE_CURRENT_LIST_DIR})
endif()
