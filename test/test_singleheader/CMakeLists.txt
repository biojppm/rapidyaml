cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml
    DESCRIPTION "Single header version of ryml library for testing"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(../../ext/c4core/cmake/c4Project.cmake)
c4_project(VERSION 0.10.0
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

# amalgamate ryml to get the single header
function(amalgamate_ryml header_dir header_file basename)
    set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
    set(singleheaderdir "${rymldir}/src_singleheader")
    set(singleheader "${singleheaderdir}/ryml_all.hpp")
    set(amscript "${rymldir}/tools/amalgamate.py")
    file(GLOB_RECURSE srcfiles
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS "${rymldir}/src")
    add_custom_command(OUTPUT "${singleheader}"
        COMMAND python "${amscript}" --events tree -- "${singleheader}"
        DEPENDS ${srcfiles} "${amscript}" "${rymldir}/ext/c4core/cmake/amalgamate_utils.py")
    set(${header_dir} "${singleheaderdir}" PARENT_SCOPE)
    set(${header_file} "${singleheader}" PARENT_SCOPE)
endfunction()
amalgamate_ryml(header_dir header_file ryml_all.hpp)

c4_add_library(ryml
    INC_DIRS
        $<BUILD_INTERFACE:${header_dir}>
        $<INSTALL_INTERFACE:include>
    SOURCE_ROOT ${header_dir}
    SOURCES
        ${header_file}
        ${CMAKE_CURRENT_LIST_DIR}/libryml_singleheader.cpp)
target_compile_definitions(ryml PUBLIC -DRYML_SINGLE_HEADER) # this is needed only for the tests, not ryml itself

include(../../ext/testbm.cmake)

enable_testing()
set(RYML_DEFINED_FROM_SINGLEHEADER ON)
add_subdirectory(../../test test)
