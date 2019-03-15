c4_setup_testing(ryml)

c4_add_library(RYML ryml_util_test
    SOURCES test_case.hpp test_case.cpp libyaml.hpp
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${ryml_yaml_cpp_inc}
    LIBS ryml yaml yaml-cpp gtest gtest_main
    FOLDER ryml-test)

function(ryml_add_test test_name)
    # NOTE: the library order here is fragile because the sanitization code
    # goes recursively through the target and its dependencies to figure out
    # the include paths, and some of them (notably from libyaml) use a
    # generator expression $<INSTALL_INTERFACE:include> which for some reason
    # cmake resolves to an empty string; after this, clang_tidy fails to find
    # some includes. The way that this could be overcome was by respecifying
    # the libraries although technically it was not necessary for correct
    # compilation and linking. This is happening only withh the clang-tidy
    # sanitizer.
    set(t ryml-test-${test_name})
    c4_add_executable(RYML ${t}
        SOURCES ${test_name}.cpp ${ARGN}
        LIBS ryml yaml-cpp ryml_util_test  # FRAGILE! see the NOTE above
        FOLDER ryml-test)
    if(RYML_DBG)
        target_compile_definitions(${t} PRIVATE RYML_DBG)
    endif()
    c4_add_test(RYML ${t} ON)
endfunction()

function(ryml_add_test_case_group name)
    ryml_add_test(${name} test_group.cpp test_group.hpp)
endfunction()

ryml_add_test(basic)
ryml_add_test_case_group(empty_file)
ryml_add_test_case_group(empty_doc)
ryml_add_test_case_group(simple_doc)
ryml_add_test_case_group(empty_map)
ryml_add_test_case_group(empty_seq)
ryml_add_test_case_group(simple_map)
ryml_add_test_case_group(simple_seq)
ryml_add_test_case_group(single_quoted)
ryml_add_test_case_group(double_quoted)
ryml_add_test_case_group(plain_scalar)
ryml_add_test_case_group(block_literal)
ryml_add_test_case_group(block_folded)
ryml_add_test_case_group(tag_property)
ryml_add_test_case_group(complex_key)
ryml_add_test_case_group(nested_mapx2)
ryml_add_test_case_group(nested_seqx2)
ryml_add_test_case_group(nested_mapx3)
ryml_add_test_case_group(nested_seqx3)
ryml_add_test_case_group(nested_mapx4)
ryml_add_test_case_group(nested_seqx4)
ryml_add_test_case_group(map_of_seq)
ryml_add_test_case_group(seq_of_map)
ryml_add_test_case_group(generic_map)
ryml_add_test_case_group(generic_seq)
ryml_add_test_case_group(scalar_names)
ryml_add_test_case_group(simple_anchor)
ryml_add_test_case_group(indentation)
ryml_add_test_case_group(number)
ryml_add_test_case_group(null_val)
ryml_add_test_case_group(github_issues)


#-------------------------------------------------------------------------

# run every case in the yaml-test-suite
option(RYML_TEST_SUITE "Enable ~300 extensive tests from yaml-test-suite." OFF)

if(RYML_TEST_SUITE)

    c4_import_remote_proj(ryml c4log ${ryml_ext_dir}/c4log
        GIT_REPOSITORY https://github.com/biojppm/c4log
        GIT_TAG master)

    c4_download_remote_proj(ryml yaml-test-suite ${ryml_ext_dir}/yaml-test-suite
        GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
        GIT_TAG master)

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
