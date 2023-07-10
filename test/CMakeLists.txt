c4_setup_testing(GTEST)


c4_add_executable(ryml-test-quickstart
    SOURCES ../samples/quickstart.cpp
    LIBS ryml
    FOLDER test)
if(RYML_DBG)
    target_compile_definitions(ryml-test-quickstart PRIVATE RYML_DBG)
endif()
c4_target_compile_flags(ryml-test-quickstart PUBLIC GCC -Wno-useless-cast)
c4_add_test(ryml-test-quickstart)


if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.0.0)
        set_property(SOURCE test_case.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-stringop-overflow)
    endif()
endif()
c4_add_library(ryml-_testlib LIBRARY_TYPE STATIC
    SOURCES test_case.hpp test_case.cpp callbacks_tester.hpp
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR}
    LIBS ryml c4fs gtest
    FOLDER test)
if(RYML_DBG)
    target_compile_definitions(ryml-_testlib PUBLIC RYML_DBG)
endif()


function(ryml_add_test test_name)
    set(t ryml-test-${test_name})
    c4_add_executable(${t}
        SOURCES test_${test_name}.cpp
        LIBS ${ARGN} ryml-_testlib gtest_main
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


function(ryml_get_target_exe target_name target_file)
    if(CMAKE_CROSSCOMPILING)
        set(tgt ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${target_name}>)
    else()
        set(tgt $<TARGET_FILE:${target_name}>)
    endif()
    set(${target_file} ${tgt} PARENT_SCOPE)
endfunction()


ryml_add_test(callbacks)
ryml_add_test(stack)
ryml_add_test(filter)
ryml_add_test(parser)
ryml_add_test(tree)
ryml_add_test(noderef)
ryml_add_test(emit)
ryml_add_test(style)
ryml_add_test(serialize)
ryml_add_test(basic)
ryml_add_test(json)
ryml_add_test(preprocess)
ryml_add_test(merge)
ryml_add_test(location)
ryml_add_test(yaml_events)
ryml_add_test_case_group(empty_file)
ryml_add_test_case_group(empty_map)
ryml_add_test_case_group(empty_seq)
ryml_add_test_case_group(empty_scalar)
ryml_add_test_case_group(simple_doc)
ryml_add_test_case_group(simple_map)
ryml_add_test_case_group(simple_seq)
ryml_add_test_case_group(simple_set)
ryml_add_test_case_group(single_quoted)
ryml_add_test_case_group(double_quoted)
ryml_add_test_case_group(plain_scalar)
ryml_add_test_case_group(block_literal)
ryml_add_test_case_group(block_folded)
ryml_add_test_case_group(tag_property)
ryml_add_test_case_group(explicit_key)
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
# test the tools as well

option(RYML_TEST_TOOLS "Enable tests for the tools. Requires file system access." ON)
if(RYML_TEST_TOOLS)
    if(NOT RYML_BUILD_TOOLS)
        add_subdirectory(../tools tools)
    endif()
    add_dependencies(ryml-test-build ryml-parse-emit)
    add_dependencies(ryml-test-build ryml-yaml-events)

    # parse & emit
    if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
        c4_err("could not find test file")
    endif()
    ryml_get_target_exe(ryml-parse-emit RYML_TGT_PARSE_EMIT)
    add_test(NAME ryml-test-tool-parse_emit COMMAND ${RYML_TGT_PARSE_EMIT} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)

    # events emitter
    function(ryml_create_file name contents fileout)
        set(filename ${CMAKE_CURRENT_BINARY_DIR}/${name})
        file(WRITE "${filename}" "${contents}
")
        set("${fileout}" "${filename}" PARENT_SCOPE)
    endfunction()
    function(ryml_add_event_tool_test name expect_success contents)
        ryml_create_file(${name}.yml "${contents}" file)
        add_test(NAME ryml-test-tool-events-${name} COMMAND ${RYML_TGT_EVENTS} ${file})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-events-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    ryml_get_target_exe(ryml-yaml-events RYML_TGT_EVENTS)
    ryml_add_event_tool_test(success TRUE "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(fail_squo FALSE "foo: 'bar")
    ryml_add_event_tool_test(fail_dquo FALSE "foo: \"bar")
    ryml_add_event_tool_test(fail_seq1 FALSE "[ a, b, c ] ]")
    ryml_add_event_tool_test(fail_seq2 FALSE "[ [a, b, c ]")
endif()


#-------------------------------------------------------------------------

# run every case in the yaml-test-suite
option(RYML_TEST_SUITE "Enable cases from yaml-test-suite, https://github.com/yaml/yaml-test-suite." ON)

if(RYML_TEST_SUITE)
    set(ed ${CMAKE_CURRENT_BINARY_DIR}/subprojects) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)

    c4_require_subproject(c4log REMOTE
        GIT_REPOSITORY https://github.com/biojppm/c4log
        GIT_TAG master)

    set(tsdir ${ed}/yaml-test-suite)
    c4_download_remote_proj(yaml-test-suite suite_dir
        GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
        GIT_TAG data-2022-01-17)
    if(NOT EXISTS ${suite_dir}/229Q)
        c4_err("cannot find yaml-test-suite at ${suite_dir} -- was there an error downloading the project?")
    endif()

    c4_add_executable(ryml-test-suite
        SOURCES
            test_suite.cpp
            test_suite/test_suite_common.hpp
            test_suite/test_suite_events_emitter.cpp
            test_suite/test_suite_events.cpp
            test_suite/test_suite_events.hpp
            test_suite/test_suite_parts.cpp
            test_suite/test_suite_parts.hpp
        LIBS ryml-_testlib c4log
        FOLDER test)
    add_dependencies(ryml-test-build ryml-test-suite)

    ryml_get_target_exe(ryml-test-suite tgt)
    function(ryml_add_test_from_suite event_file)
        get_filename_component(case_dir ${event_file} DIRECTORY)
        string(REPLACE "\\" "_" case_name "${case_dir}")
        string(REPLACE "/" "_" case_name "${case_name}")
        file(GLOB case_files RELATIVE "${suite_dir}/${case_dir}" "${suite_dir}/${case_dir}/*")
        #message("${case_name}: ${case_dir} ${event_file} ${case_files}")
        if(NOT EXISTS "${suite_dir}/${case_dir}/error")
            foreach(case_file ${case_files})
                string(REPLACE "." "_" approach "${case_file}")
                set(test_name ${case_name}-${approach})
                #message("${test_name}: ${case_name}: ${case_dir} ${case_file}")
                if("${case_file}" STREQUAL "===")
                    continue()
                elseif("${case_file}" STREQUAL "test.event")
                    continue()
                elseif("${case_file}" STREQUAL "lex.token")
                    continue()
                elseif("${case_file}" STREQUAL "error")
                    continue()
                elseif("${case_file}" STREQUAL "in.json")
                    add_test(NAME ryml-test-suite-${test_name} COMMAND ${tgt} "${test_name}" "${suite_dir}/${case_dir}" in.json "--gtest_filter=-*events*:-*check_expected_error")
                else()
                    add_test(NAME ryml-test-suite-${test_name} COMMAND ${tgt} "${test_name}" "${suite_dir}/${case_dir}" "${case_file}" "--gtest_filter=-*events*:-*check_expected_error")
                    add_test(NAME ryml-test-suite-${test_name}-events COMMAND ${tgt} "${test_name}-events" "${suite_dir}/${case_dir}" "${case_file}" "--gtest_filter=*events*")
                endif()
            endforeach()
        else()
            set(test_name ${case_name}-error)
            add_test(NAME ryml-test-suite-${test_name} COMMAND ${tgt} "${test_name}" "${suite_dir}/${case_dir}" in.yaml "--gtest_filter=*check_expected_error")
        endif()
    endfunction()

    file(GLOB_RECURSE event_files RELATIVE "${suite_dir}" "${suite_dir}/*.event")
    foreach(case ${event_files})
        ryml_add_test_from_suite(${case})
    endforeach()
endif(RYML_TEST_SUITE)
