if(NOT _ext_c4core_cmake_included)
set(_ext_c4core_cmake_included ON)


function(ryml_add_c4core_src_to_target target)
    _ryml_add_c4core_to_target(${target} c4core.src ON)
endfunction()


function(ryml_add_c4core_dev_to_target target)
    if(NOT TARGET ryml-_c4core_dev)
        c4_add_library(ryml-_c4core_dev LIBRARY_TYPE STATIC)
        _ryml_add_c4core_to_target(ryml-_c4core_dev c4core.dev OFF)
        target_include_directories(ryml-_c4core_dev PUBLIC
            ${_ryml_ext_dir}/c4core.src
            ${_ryml_ext_dir}/c4core.dev
        )
        target_link_libraries(ryml-_c4core_dev PUBLIC ryml)
    endif()
    target_link_libraries(${target} PUBLIC ryml-_c4core_dev)
endfunction()


set(_ryml_ext_dir ${CMAKE_CURRENT_LIST_DIR})  # see https://stackoverflow.com/questions/12802377/in-cmake-how-can-i-find-the-directory-of-an-included-file


function(_ryml_add_c4core_to_target target whichc4core doinstall)
    set(_listfile ${_ryml_ext_dir}/${whichc4core}.txt)
    set(_srcdir ${_ryml_ext_dir}/${whichc4core})
    file(STRINGS ${_listfile} _src)
    _c4_transform_to_full_path(_src _ ${_srcdir})
    target_sources(${target} PRIVATE ${_src})
    #create_source_group("" "${_srcdir}" "${_src}")
    if(${doinstall})
        list(FILTER _src EXCLUDE REGEX .*cpp)
        c4_install_files("${_src}" ${CMAKE_INSTALL_PREFIX}/include ${_srcdir})
    endif()
    target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${_srcdir}>)
    get_target_property(_target_type ${target} TYPE)
    if(WIN32 AND (_target_type STREQUAL "SHARED_LIBRARY"))
        target_compile_definitions(${target} PUBLIC C4CORE_SHARED)
        target_compile_definitions(${target} PRIVATE $<BUILD_INTERFACE:C4CORE_EXPORTS>)
    endif()
endfunction()

endif(NOT _ext_c4core_cmake_included)
