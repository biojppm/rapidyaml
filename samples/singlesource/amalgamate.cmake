find_package(Python3 COMPONENTS Interpreter)

# amalgamate ryml to get the single header
function(amalgamate_ryml header_dir header_file source_file)
    set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
    set(singleheaderdir "${rymldir}/tools/amalgamate/singlesrc")
    set(singleheader "${singleheaderdir}/ryml_all.hpp")
    set(singlesource "${singleheaderdir}/ryml_all.cpp")
    set(amscript "${rymldir}/tools/amalgamate.py")
    file(MAKE_DIRECTORY ${singleheaderdir})
    file(GLOB_RECURSE srcfiles
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS "${rymldir}/src")
    add_custom_command(OUTPUT "${singleheader}" "${singlesource}"
        COMMAND "${Python3_EXECUTABLE}" "${amscript}" --type single_src ryml_all
        COMMENT "cd ${singleheaderdir} ; ${Python3_EXECUTABLE} ${amscript} --type single_src ryml_all"
        WORKING_DIRECTORY ${singleheaderdir}
        DEPENDS ${srcfiles} "${amscript}" "${rymldir}/proj/c4proj/amalgamate_utils.py")
    add_custom_target(amalgamate DEPENDS "${singleheader}" "${singlesource}")
    # ensure this works with older cmake: GENERATED property did not
    # propagate universally on older cmake versions
    set_property(SOURCE ${singleheader} PROPERTY GENERATED 1)
    set_property(SOURCE ${singlesource} PROPERTY GENERATED 1)
    set(${header_dir} "${singleheaderdir}" PARENT_SCOPE)
    set(${header_file} "${singleheader}" PARENT_SCOPE)
    set(${source_file} "${singlesource}" PARENT_SCOPE)
endfunction()
