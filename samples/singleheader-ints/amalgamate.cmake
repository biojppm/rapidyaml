find_package(Python3 COMPONENTS Interpreter)

# amalgamate ryml to get the single header
function(amalgamate_ryml header_dir header_file)
    set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
    set(singleheaderdir "${rymldir}/src_singleheader")
    set(singleheader "${singleheaderdir}/ryml_ints.hpp")
    set(amscript "${rymldir}/tools/amalgamate.py")
    file(GLOB_RECURSE srcfiles
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS "${rymldir}/src" "${rymldir}/src_extra")
    add_custom_command(OUTPUT "${singleheader}"
        COMMAND "${Python3_EXECUTABLE}" "${amscript}" -e ints -- "${singleheader}"
        COMMENT "${Python3_EXECUTABLE} ${amscript} -e ints -- ${singleheader}"
        DEPENDS ${srcfiles} "${amscript}" "${rymldir}/ext/c4core/cmake/amalgamate_utils.py")
    set(${header_dir} "${singleheaderdir}" PARENT_SCOPE)
    set(${header_file} "${singleheader}" PARENT_SCOPE)
endfunction()
