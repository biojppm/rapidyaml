cmake_minimum_required(VERSION 3.13)
project(ryml
    DESCRIPTION "Rapid YAML parsing and emitting"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(./proj/compat.cmake)
include(./proj/c4proj/c4Project.cmake)

c4_project(VERSION 0.10.0 STANDALONE
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")


#-------------------------------------------------------

option(RYML_WITH_TAB_TOKENS "Enable parsing of tabs after ':' and '-'. This is costly and disabled by default." OFF)
option(RYML_DEFAULT_CALLBACKS "Enable ryml's default implementation of callbacks: allocate(), free(), error()" ON)
if(RYML_DEFAULT_CALLBACKS)
    option(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS "Throw exceptions instead of calling abort in the default error handler provided by ryml" OFF)
endif()
option(RYML_USE_ASSERT "Enable assertions regardless of build type. Default is only when NDEBUG is not defined (which is in release builds). This causes a slowdown of the code." OFF)
option(RYML_SHORT_ERR_MSG "Use shorter error message from checks/asserts: do not show the check condition in the error message" OFF)
option(RYML_BUILD_TOOLS "Build tools" OFF)
option(RYML_BUILD_API "Enable API generation (python, etc)" OFF)
option(RYML_DBG "Enable (very verbose) debug prints." OFF)

option(RYML_INSTALL "Enable install target" ON)


#-------------------------------------------------------

c4_add_library(ryml
    SOURCES
        ryml.hpp
        ryml_std.hpp
        ryml.natvis
        c4/yml/detail/checks.hpp
        c4/yml/detail/dbgprint.hpp
        c4/yml/detail/print.hpp
        c4/yml/detail/stack.hpp
        c4/yml/common.hpp
        c4/yml/common.cpp
        c4/yml/emit.def.hpp
        c4/yml/emit.hpp
        c4/yml/error.hpp
        c4/yml/error.def.hpp
        c4/yml/escape_scalar.hpp
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
    SOURCE_ROOT ${RYML_SRC_DIR}
    INC_DIRS
        $<BUILD_INTERFACE:${RYML_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
    )

if(NOT RYML_STANDALONE)
    target_link_libraries(ryml PUBLIC c4core)
else()
    function(_ryml_add_sources_to_lib lib filelist filedir)
        file(STRINGS ${filelist} _src)
        _c4_transform_to_full_path(_src _ ${filedir})
        target_sources(${lib} PRIVATE ${_src})
        target_include_directories(${lib} PUBLIC $<BUILD_INTERFACE:${filedir}>)
        list(FILTER _src EXCLUDE REGEX .*cpp)
        c4_install_files("${_src}" ${CMAKE_INSTALL_PREFIX}/include ${filedir})
    endfunction()
    _ryml_add_sources_to_lib(ryml ext/c4core.src.txt ${CMAKE_CURRENT_LIST_DIR}/ext/c4core.src)
    if(RYML_BUILD_TESTS OR RYML_BUILD_BENCHMARKS)
        _ryml_add_sources_to_lib(ryml ext/c4core.dev.txt ${CMAKE_CURRENT_LIST_DIR}/ext/c4core.dev)
    endif()
endif()

if(RYML_WITH_TAB_TOKENS)
    target_compile_definitions(ryml PUBLIC RYML_WITH_TAB_TOKENS)
endif()

if(NOT RYML_DEFAULT_CALLBACKS)
    target_compile_definitions(ryml PRIVATE RYML_NO_DEFAULT_CALLBACKS)
else()
    if(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS)
        target_compile_definitions(ryml PUBLIC RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS)
    endif()
endif()

if(RYML_SHORT_CHECK_MSG)
    target_compile_definitions(ryml PUBLIC RYML_SHORT_CHECK_MSG)
endif()
if(RYML_USE_ASSERT)
    target_compile_definitions(ryml PUBLIC RYML_USE_ASSERT=1)
endif()

if(RYML_DBG)
    target_compile_definitions(ryml PRIVATE RYML_DBG)
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
    if(RYML_STANDALONE)
        c4_install_exports()
    else()
        c4_install_exports(DEPENDENCIES c4core)
    endif()
    c4_pack_project()
endif()

# generate and install the install manifest
# https://stackoverflow.com/questions/66752091/is-there-a-way-in-cmake-to-save-away-the-install-manifest-txt-for-later
install(CODE "
string(REPLACE \"\$\{CMAKE_INSTALL_PREFIX\}/\" \"\" ryml_manifest \"\$\{CMAKE_INSTALL_MANIFEST_FILES\}\")\n\
list(APPEND ryml_manifest share/ryml/MANIFEST.txt)\n\
list(SORT ryml_manifest)\n\
string(REPLACE \";\" \"\\n\" ryml_manifest \"\$\{ryml_manifest\}\")\n\
file(WRITE ${CMAKE_BINARY_DIR}/MANIFEST.txt \"\$\{ryml_manifest\}\")
")
install(FILES "${CMAKE_BINARY_DIR}/MANIFEST.txt" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ryml)

# add a target to uninstall
add_custom_target(ryml-uninstall
    "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/proj/uninstall.cmake"
)


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
