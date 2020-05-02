c4_setup_testing()

c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER test)

c4_add_library(ryml-_testlib LIBRARY_TYPE STATIC
    SOURCES test_case.hpp test_case.cpp libyaml.hpp
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${ryml_yaml_cpp_inc}
    LIBS ryml c4fs yaml yaml-cpp gtest gtest_main
    FOLDER test)

function(ryml_add_test test_name)
    # NOTE: the library order here is fragile because the c4proj sanitization
    # code goes recursively through the target and its dependencies to figure
    # out the include paths, and some of them use a generator expression
    # $<INSTALL_INTERFACE:include>, which for some reason cmake resolves to
    # an empty string; that results in an empty -I directive: for example "-I
    # -I other/path", which prevents clang-tidy from considering
    # other/path. The way that this could be overcome was by respecifying the
    # libraries, although technically it was not necessary for correct
    # compilation and linking. This is happening only with the clang-tidy
    # sanitizer.
    set(t ryml-test-${test_name})
    c4_add_executable(${t}
        SOURCES ${test_name}.cpp
        LIBS ${ARGN} ryml yaml-cpp ryml-_testlib yaml-cpp ryml # FRAGILE! see the NOTE above
        FOLDER test)
    if(RYML_DBG)
        target_compile_definitions(${t} PRIVATE RYML_DBG)
    endif()
    c4_add_test(${t})
endfunction()

c4_add_library(ryml-_testgroup LIBRARY_TYPE OBJECT
    SOURCES test_group.cpp test_group.hpp
    LIBS ryml ryml-_testlib c4fs
    FOLDER test)

function(ryml_add_test_case_group name)
    ryml_add_test(${name} ryml-_testgroup)
endfunction()

ryml_add_test(basic)
ryml_add_test(stack)
ryml_add_test(basic_json)
ryml_add_test(preprocess)
ryml_add_test(merge)
ryml_add_test_case_group(empty_file)
ryml_add_test_case_group(empty_doc)
ryml_add_test_case_group(simple_doc)
ryml_add_test_case_group(empty_map)
ryml_add_test_case_group(empty_seq)
ryml_add_test_case_group(simple_map)
ryml_add_test_case_group(simple_seq)
ryml_add_test_case_group(simple_set)
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

c4_add_install_include_test(ryml "ryml::")
c4_add_install_link_test(ryml "ryml::" "
#include <c4/yml/yml.hpp>

int main()
{
    auto tree = c4::yml::parse(R\"(
a: 1
b: {b0: 2, b1: 3}
c: [4, 5, 6]
)\");
    #define check(cond) if(!(cond)) { return __LINE__; }
    check(!tree.empty())
    check(tree.rootref().is_map())
    check(tree.rootref().num_children() == 3)
    check(tree[\"a\"].is_keyval())
    check(tree[\"a\"].num_children() == 0)
    check(tree[\"a\"].val() == \"1\")
    check(tree[\"b\"].is_map())
    check(tree[\"b\"].num_children() == 2)
    check(tree[\"b\"][\"b0\"].val() == \"2\")
    check(tree[\"b\"][\"b1\"].val() == \"3\")
    check(tree[\"c\"].is_seq())
    check(tree[\"c\"].num_children() == 3)
    check(tree[\"c\"][0].val() == \"4\")
    check(tree[\"c\"][1].val() == \"5\")
    check(tree[\"c\"][2].val() == \"6\")
    return 0;
}
")


#-------------------------------------------------------------------------

# run every case in the yaml-test-suite
option(RYML_TEST_SUITE "Enable ~300 extensive test cases from yaml-test-suite, https://github.com/yaml/yaml-test-suite." OFF)

if(RYML_TEST_SUITE)
    set(ed ${CMAKE_CURRENT_BINARY_DIR}/ext) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)

    c4_require_subproject(c4log REMOTE
        GIT_REPOSITORY https://github.com/biojppm/c4log
        GIT_TAG master)

    c4_download_remote_proj(yaml-test-suite ${ed}/yaml-test-suite
        GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
        GIT_TAG master)

    set(suite_dir ${ed}/yaml-test-suite/src/test)
    if(NOT EXISTS ${suite_dir})
        c4_err("cannot find yaml-test-suite -- was there an error downloading the project?")
    endif()

    c4_add_executable(ryml-test-suite
        SOURCES suite.cpp
        LIBS ryml c4fs c4log gtest
        FOLDER test)
    if(RYML_DBG)
        target_compile_definitions(ryml-test-suite PRIVATE RYML_DBG)
    endif()

    set(tgt $<TARGET_FILE:ryml-test-suite>)
    function(ryml_add_test_from_suite tml_file)
        get_filename_component(name ${tml_file} NAME_WE)
        add_test(NAME ryml-test-suite-${name}-in_yaml
            COMMAND ${tgt} --gtest_filter="*/in_yaml*" ${suite_dir}/${tml_file})
        add_test(NAME ryml-test-suite-${name}-out_yaml
            COMMAND ${tgt} --gtest_filter="*/out_yaml*" ${suite_dir}/${tml_file})
        add_test(NAME ryml-test-suite-${name}-events
            COMMAND ${tgt} --gtest_filter="*/events*" ${suite_dir}/${tml_file})
        add_test(NAME ryml-test-suite-${name}-in_json
            COMMAND ${tgt} --gtest_filter="*/in_json*" ${suite_dir}/${tml_file})
    endfunction()

    file(GLOB suite_cases RELATIVE "${suite_dir}" "${suite_dir}/*.tml")
    foreach(case ${suite_cases})
        ryml_add_test_from_suite(${case})
    endforeach()

endif(RYML_TEST_SUITE)
