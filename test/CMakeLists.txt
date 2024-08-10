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


c4_add_library(ryml-_testlib LIBRARY_TYPE STATIC
    SOURCES
       test_lib/callbacks_tester.hpp
       test_lib/test_case_node.hpp
       test_lib/test_case_node.cpp
       test_lib/test_case.hpp
       test_lib/test_case.cpp
       test_lib/test_engine.hpp
       test_lib/test_engine.cpp
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
    c4_add_test(${t})
endfunction()


c4_add_library(ryml-_testgroup LIBRARY_TYPE STATIC
    SOURCES
       test_lib/test_group.hpp
       test_lib/test_group.def.hpp
       test_lib/test_group.cpp
    LIBS ryml ryml-_testlib c4fs
    FOLDER test)
if(RYML_DBG)
    target_compile_definitions(ryml-_testgroup PUBLIC RYML_DBG)
endif()
function(ryml_add_test_case_group name)
    ryml_add_test(${name})
    target_link_libraries(ryml-test-${name} PUBLIC ryml-_testgroup)
endfunction()


c4_add_library(ryml-_evt_handler_yaml_std LIBRARY_STATIC
    SOURCES
       test_suite/test_suite_event_handler.cpp
       test_suite/test_suite_event_handler.hpp
    LIBS ryml
    FOLDER test
)
if(RYML_DBG)
    target_compile_definitions(ryml-_evt_handler_yaml_std PUBLIC RYML_DBG)
endif()
function(ryml_add_engine_test name)
    ryml_add_test(${name} ryml-_evt_handler_yaml_std)
endfunction()


ryml_add_engine_test(parse_engine_1_doc)
ryml_add_engine_test(parse_engine_2_map)
ryml_add_engine_test(parse_engine_3_seq)
ryml_add_engine_test(parse_engine_4_anchor)
ryml_add_engine_test(parse_engine_5_tag)
ryml_add_engine_test(parse_engine_6_qmrk)
ryml_add_engine_test(parse_engine_7_seqimap)
ryml_add_engine_test(parse_engine_8_scalars_tokens)
ryml_add_engine_test(yaml_events)
ryml_add_test(version)
ryml_add_test(callbacks)
ryml_add_test(stack)
ryml_add_test(filter)
ryml_add_test(parser)
ryml_add_test(node_type)
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
ryml_add_test_case_group(empty_file)
ryml_add_test_case_group(doc)
ryml_add_test_case_group(seq)
ryml_add_test_case_group(seq_empty)
ryml_add_test_case_group(seq_generic)
ryml_add_test_case_group(map)
ryml_add_test_case_group(map_empty)
ryml_add_test_case_group(map_generic)
ryml_add_test_case_group(map_set)
ryml_add_test_case_group(seq_of_map)
ryml_add_test_case_group(map_of_seq)
ryml_add_test_case_group(scalar_empty)
ryml_add_test_case_group(scalar_null)
ryml_add_test_case_group(scalar_squoted)
ryml_add_test_case_group(scalar_dquoted)
ryml_add_test_case_group(scalar_literal)
ryml_add_test_case_group(scalar_folded)
ryml_add_test_case_group(scalar_plain)
ryml_add_test_case_group(tag_property)
ryml_add_test_case_group(explicit_key)
ryml_add_test_case_group(map_nestedx2)
ryml_add_test_case_group(seq_nestedx2)
ryml_add_test_case_group(map_nestedx3)
ryml_add_test_case_group(seq_nestedx3)
ryml_add_test_case_group(map_nestedx4)
ryml_add_test_case_group(seq_nestedx4)
ryml_add_test_case_group(scalar_names)
ryml_add_test_case_group(anchor)
ryml_add_test_case_group(indentation)
ryml_add_test_case_group(number)
ryml_add_test_case_group(github_issues)


#-------------------------------------------------------------------------
# test the tools as well

function(ryml_get_target_exe target_name target_file)
    if(CMAKE_CROSSCOMPILING)
        set(tgt ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${target_name}>)
    else()
        set(tgt $<TARGET_FILE:${target_name}>)
    endif()
    set(${target_file} ${tgt} PARENT_SCOPE)
endfunction()

if(NOT EMSCRIPTEN)
    option(RYML_TEST_TOOLS "Enable tests for the tools. Requires file system access." ON)
endif()

if(RYML_TEST_TOOLS)
    if(NOT RYML_BUILD_TOOLS)
        add_subdirectory(../tools tools)
    endif()
    add_dependencies(ryml-test-build ryml-parse-emit)
    add_dependencies(ryml-test-build ryml-yaml-events)
    #
    # parse & emit
    if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
        c4_err("could not find test file")
    endif()
    ryml_get_target_exe(ryml-parse-emit RYML_TGT_PARSE_EMIT)
    add_test(NAME ryml-test-tool-parse_emit COMMAND ${RYML_TGT_PARSE_EMIT} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
    #
    # events emitter
    function(ryml_create_file name contents fileout)
        set(filename ${CMAKE_CURRENT_BINARY_DIR}/${name})
        file(WRITE "${filename}" "${contents}
")
        set("${fileout}" "${filename}" PARENT_SCOPE)
    endfunction()
    function(ryml_add_event_tool_test name expect_success flags contents)
        ryml_create_file(${name}.yml "${contents}" file)
        add_test(NAME ryml-test-tool-events-${name} COMMAND ${RYML_TGT_EVENTS} ${flags} ${file})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-events-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    ryml_get_target_exe(ryml-yaml-events RYML_TGT_EVENTS)
    ryml_add_event_tool_test(success_tree TRUE "-t" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(success_evts TRUE ""   "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_squo_tree FALSE "-t" "foo: 'bar")
    ryml_add_event_tool_test(fail_squo_evts FALSE ""   "foo: 'bar")
    ryml_add_event_tool_test(fail_dquo_tree FALSE "-t" "foo: \"bar")
    ryml_add_event_tool_test(fail_dquo_evts FALSE ""   "foo: \"bar")
    ryml_add_event_tool_test(fail_seq1_tree FALSE "-t" "[ a, b, c ] ]")
    ryml_add_event_tool_test(fail_seq2_evts FALSE ""   "[ [a, b, c ]")
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
            test_suite/test_suite_event_handler.cpp
            test_suite/test_suite_event_handler.hpp
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
                #message("${test_name}: ${case_name} ${case_dir} ${case_file}")
                set(cmd_with_args ${tgt} "${test_name}" "${suite_dir}/${case_dir}" "${case_file}")
                if("${case_file}" STREQUAL "===")
                    continue()
                elseif("${case_file}" STREQUAL "test.event")
                    continue()
                elseif("${case_file}" STREQUAL "lex.token")
                    continue()
                elseif("${case_file}" STREQUAL "error")
                    continue()
                elseif("${case_file}" STREQUAL "in.yaml")
                    add_test(NAME ryml-test-suite-${test_name}        COMMAND ${cmd_with_args} "--gtest_filter=-*events*:-*check_expected_error*")
                    add_test(NAME ryml-test-suite-${test_name}-events COMMAND ${cmd_with_args} "--gtest_filter=*events*")
                elseif("${case_file}" STREQUAL "out.yaml")
                    add_test(NAME ryml-test-suite-${test_name}        COMMAND ${cmd_with_args} "--gtest_filter=-*events*:-*check_expected_error*")
                    add_test(NAME ryml-test-suite-${test_name}-events COMMAND ${cmd_with_args} "--gtest_filter=-*ref_events*:*events*")
                elseif("${case_file}" STREQUAL "emit.yaml")
                    add_test(NAME ryml-test-suite-${test_name}        COMMAND ${cmd_with_args} "--gtest_filter=-*events*:-*check_expected_error*")
                    add_test(NAME ryml-test-suite-${test_name}-events COMMAND ${cmd_with_args} "--gtest_filter=-*ref_events*:*events*")
                elseif("${case_file}" STREQUAL "in.json")
                    add_test(NAME ryml-test-suite-${test_name}        COMMAND ${cmd_with_args} "--gtest_filter=-*events*:-*check_expected_error*")
                else()
                    c4_err("unknown file: ${case_file}")
                endif()
            endforeach()
        else()
            set(test_name ${case_name}-error)
            add_test(NAME ryml-test-suite-${test_name} COMMAND ${tgt} "${test_name}" "${suite_dir}/${case_dir}" in.yaml "--gtest_filter=*check_expected_error*:*check_expected_error*ref_events")
        endif()
    endfunction()

    file(GLOB_RECURSE event_files RELATIVE "${suite_dir}" "${suite_dir}/*.event")
    foreach(case ${event_files})
        ryml_add_test_from_suite(${case})
    endforeach()
endif(RYML_TEST_SUITE)


#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

# run every known fuzz crash
set(fuzzdefault ON)
if(EMSCRIPTEN)
    set(fuzzdefault OFF)
endif()
option(RYML_TEST_FUZZ "Enable tests for known problematic fuzz cases." ${fuzzdefault})

if(RYML_TEST_FUZZ)
    c4_download_remote_proj(rapidyaml-data rapidyaml_data_dir
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml-data
        GIT_TAG master)
    if(NOT EXISTS ${rapidyaml_data_dir}/fuzz/yaml.dict)
        c4_err("cannot find rapidyaml-data at ${rapidyaml_data_dir} -- was there an error downloading the project?")
    endif()

    set(corpus_suite_dir ${rapidyaml_data_dir}/fuzz/yaml_test_suite)
    set(corpus_generated_dir ${rapidyaml_data_dir}/fuzz/yaml_generated)
    set(corpus_artifacts_dir ${rapidyaml_data_dir}/fuzz/yaml_artifacts)
    set(corpus_merged_dir ${rapidyaml_data_dir}/fuzz/yaml_merged)
    set(yaml_dict ${rapidyaml_data_dir}/fuzz/yaml.dict)
    file(GLOB_RECURSE fuzz_files RELATIVE "${corpus_artifacts_dir}" "${corpus_artifacts_dir}/*")
    file(GLOB_RECURSE suite_files RELATIVE "${corpus_suite_dir}" "${corpus_suite_dir}/*")

    # add individual tests for problematic fuzz files
    function(ryml_add_fuzz_test name)
        c4_add_executable(ryml-test-fuzz-${name}
            SOURCES
            test_fuzz/test_fuzz_common.hpp
            test_fuzz/test_fuzz_${name}.cpp
            test_fuzz/test_fuzz_main.cpp
            ${ARGN}
            INC_DIRS ${CMAKE_CURRENT_LIST_DIR}
            LIBS ryml c4fs
            FOLDER test/fuzz)
        if(RYML_DBG)
            target_compile_definitions(ryml-test-fuzz-${name} PUBLIC RYML_DBG)
        endif()
        add_dependencies(ryml-test-build ryml-test-fuzz-${name})
        ryml_get_target_exe(ryml-test-fuzz-${name} tgtexe)
        function(ryml_add_fuzz_test_file name_ dir file)
            string(REPLACE "/" "_" fuzz_name "${file}")
            add_test(NAME ryml-test-fuzz-${name_}-${fuzz_name}
                COMMAND ${tgtexe} ${dir}/${file})
        endfunction()
        foreach(fuzz_file ${fuzz_files})
            ryml_add_fuzz_test_file(${name} ${corpus_artifacts_dir} ${fuzz_file})
        endforeach()
    endfunction()
    ryml_add_fuzz_test(parse_emit)
    ryml_add_fuzz_test(events
        ../test/test_suite/test_suite_event_handler.hpp
        ../test/test_suite/test_suite_event_handler.cpp)
endif()
