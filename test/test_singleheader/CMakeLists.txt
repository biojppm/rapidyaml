cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml
    DESCRIPTION "Single header version of ryml library for testing"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(../../proj/c4proj/c4Project.cmake)
c4_project(VERSION 0.15.2
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

# amalgamate ryml to get the single header
function(amalgamate_ryml target_name basename header_dir header_file)
    set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
    set(singleheaderdir "${rymldir}/src_singleheader")
    set(singleheader "${singleheaderdir}/${basename}")
    set(amscript "${rymldir}/tools/amalgamate.py")
    file(GLOB_RECURSE srcfiles
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS "${rymldir}/src")
    # do not use add_custom_command because there are parallel
    # dependencies
    add_custom_target(${target_name}
        COMMAND python "${amscript}" --events all -- "${singleheader}"
        BYPRODUCTS "${singleheader}"
        DEPENDS ${srcfiles} "${amscript}" "${rymldir}/proj/c4proj/amalgamate_utils.py")
    set(files "${${header_file}}")
    list(APPEND files "${singleheader}")
    set(${header_dir} ${singleheaderdir} PARENT_SCOPE)
    set(${header_file} ${files} PARENT_SCOPE)
endfunction()
# workaround to: some tests use ryml_all, others ryml_ints
amalgamate_ryml(ryml_hdr_ints ryml_ints.hpp hdr_dir hdr_file_ints)
amalgamate_ryml(ryml_hdr_all  ryml_all.hpp  hdr_dir hdr_file_all)


c4_add_library(ryml
    INC_DIRS
        $<BUILD_INTERFACE:${hdr_dir}>
        $<INSTALL_INTERFACE:include>
    SOURCE_ROOT ${hdr_dir}
    SOURCES
        ${hdr_file_all}
        ${CMAKE_CURRENT_LIST_DIR}/libryml_singleheader.cpp)
target_compile_definitions(ryml PUBLIC -DRYML_SINGLE_HEADER) # this is needed only for the tests, not ryml itself

set(RYML_DEFINED_FROM_SINGLEHEADER ON)

enable_testing()
include(../../ext/c4core.cmake)
include(../../ext/testbm.cmake)
function(ryml_maybe_dump_test_yaml target)
    # nothing to do
endfunction()
add_subdirectory(../../test test)
