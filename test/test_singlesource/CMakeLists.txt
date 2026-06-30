cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml
    DESCRIPTION "Single header version of ryml library for testing"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(../../proj/c4proj/c4Project.cmake)
c4_project(VERSION 0.15.2
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

# amalgamate ryml to get the single header
function(amalgamate_ryml target_name basename header_dir header_file source_file)
    set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
    set(singleheaderdir "${rymldir}/tools/amalgamate/singlesrc")
    set(singleheader "${singleheaderdir}/${basename}.hpp")
    set(singlesource "${singleheaderdir}/${basename}.cpp")
    set(amscript "${rymldir}/tools/amalgamate.py")
    file(MAKE_DIRECTORY "${singleheaderdir}")
    file(GLOB_RECURSE srcfiles
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS "${rymldir}/src")
    # do not use add_custom_command because there are parallel
    # dependencies
    add_custom_target(${target_name}
        COMMAND python "${amscript}" --events all --type single_src -- ${basename}
        COMMENT "cd ${singleheaderdir} ; python ${amscript} --events all --type single_src -- ${basename}"
        WORKING_DIRECTORY "${singleheaderdir}"
        BYPRODUCTS "${singleheader}" "${singlesource}"
        DEPENDS ${srcfiles} "${amscript}" "${rymldir}/proj/c4proj/amalgamate_utils.py")
    set(${header_dir} ${singleheaderdir} PARENT_SCOPE)
    set(${header_file} ${singleheader} PARENT_SCOPE)
    set(${source_file} ${singlesource} PARENT_SCOPE)
endfunction()
# workaround to: some tests use ryml_all, others ryml_ints
amalgamate_ryml(ryml_hdr_ints ryml_ints hdr_dir hdr_file_ints src_file_ints)
amalgamate_ryml(ryml_hdr_all  ryml_all  hdr_dir hdr_file_all src_file_all)

c4_add_library(ryml
    INC_DIRS
        $<BUILD_INTERFACE:${hdr_dir}>
        $<INSTALL_INTERFACE:include>
    SOURCE_ROOT ${hdr_dir}
    SOURCES
        ${hdr_file_all} ${src_file_all}
        ${hdr_file_ints} # <- not actually part of the target.
                         # this is done to ensure the file is generated
                         # because it is used in some tests
     )
target_compile_definitions(ryml PUBLIC -DRYML_SINGLE_HEADER) # this is needed only for the tests, not ryml itself

set(RYML_DEFINED_FROM_SINGLEHEADER ON)

enable_testing()
include(../../ext/c4core.cmake)
include(../../ext/testbm.cmake)
function(ryml_maybe_dump_test_yaml target)
    # nothing to do
endfunction()
add_subdirectory(../../test test)
