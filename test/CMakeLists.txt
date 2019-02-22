c4_setup_testing(ryml)

# a test suite for YAML
c4_download_remote_proj(ryml yaml-test-suite ${CMAKE_CURRENT_BINARY_DIR}/extern/yaml-test-suite
    GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
    GIT_TAG master
    )

# extern libraries

set(ryml_ext_dir ${CMAKE_CURRENT_BINARY_DIR}/extern)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
c4_import_remote_proj(ryml libyaml ${ryml_ext_dir}/libyaml
    GIT_REPOSITORY https://github.com/yaml/libyaml
    GIT_TAG master
    )

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
c4_import_remote_proj(ryml yaml-cpp ${ryml_ext_dir}/yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG master
    )

#-------------------------------------------------------------------------

function(ryml_add_test test_name test_src)
    set(t ryml-test-${test_name})
    c4_add_executable(RYML ${t}
        SOURCES test_case.hpp test_case.cpp ${test_src} ${c4tpl_files}
        INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${ryml_ext_dir}/yaml-cpp/src/include
        LIBS ryml yaml yaml-cpp gtest
        FOLDER ryml-test)
    if(RYML_DBG)
        target_compile_definitions(${t} PRIVATE RYML_DBG)
    endif()
    c4_add_test(RYML ${t} ON)
endfunction()

ryml_add_test(test_basic basic.cpp)

#-------------------------------------------------------------------------

# run every case in the yaml-test-suite
option(RYML_TEST_SUITE "Enable ~300 tests from yaml-test-suite.")

if(RYML_TEST_SUITE)

    c4_import_remote_proj(ryml c4fs ${ryml_ext_dir}/c4fs
        GIT_REPOSITORY https://github.com/biojppm/c4fs
        GIT_TAG master
        )

    c4_import_remote_proj(ryml c4log ${ryml_ext_dir}/c4log
        GIT_REPOSITORY https://github.com/biojppm/c4log
        GIT_TAG master
        )

    c4_download_remote_proj(ryml yaml-test-suite ${ryml_ext_dir}/yaml-test-suite
        GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
        GIT_TAG master
        )

    set(suite_dir ${ryml_ext_dir}/yaml-test-suite/src/test)
    if(NOT EXISTS ${suite_dir})
        message(FATAL_ERROR "cannot find yaml-test-suite -- was there an error downloading the project")
    endif()

    c4_add_executable(RYML ryml-test-suite
        SOURCES suite.cpp
        LIBS ryml c4fs c4log gtest
        FOLDER ryml-test)
    if(RYML_DBG)
        target_compile_definitions(ryml-test-suite PRIVATE RYML_DBG)
    endif()

    function(ryml_add_test_from_suite tml_file)
        get_filename_component(name ${tml_file} NAME_WE)
        add_test(NAME ryml-test-suite-${name}
            COMMAND $<TARGET_FILE:ryml-test-suite> ${suite_dir}/${tml_file})
    endfunction()

    file(GLOB suite_cases RELATIVE "${suite_dir}" "${suite_dir}/*.tml")
    foreach(case ${suite_cases})
        ryml_add_test_from_suite(${case})
    endforeach()

endif(RYML_TEST_SUITE)
