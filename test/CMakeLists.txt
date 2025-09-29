c4_setup_testing(GTEST)


function(ryml_add_test_no_lib test_name)
    c4_add_executable(${test_name} LIBS ryml FOLDER test ${ARGN})
    if(RYML_DBG)
        target_compile_definitions(${test_name} PRIVATE RYML_DBG)
        c4_target_compile_flags(${test_name} PUBLIC GCC -Wno-useless-cast)
    endif()
    c4_add_test(${test_name})
endfunction()
ryml_add_test_no_lib(ryml-test-quickstart
    SOURCES ../samples/quickstart.cpp)
ryml_add_test_no_lib(ryml-test-quickstart-ints
    SOURCES
        ../samples/quickstart-ints.cpp
        ../src_extra/c4/yml/extra/event_handler_ints.hpp
        ../src_extra/c4/yml/extra/event_handler_ints.cpp
    INC_DIRS ../src_extra)


c4_add_library(ryml-_testlib LIBRARY_TYPE STATIC
    SOURCES
       test_lib/callbacks_tester.hpp
       test_lib/test_case_node.hpp
       test_lib/test_case_node.cpp
       test_lib/test_case.hpp
       test_lib/test_case.cpp
       test_lib/test_engine.hpp
       test_lib/test_engine.cpp
       test_lib/test_events_ints_helpers.hpp
       test_lib/test_events_ints_helpers.cpp
       ../src_extra/c4/yml/extra/event_handler_ints.cpp
       ../src_extra/c4/yml/extra/event_handler_ints.hpp
       ../src_extra/c4/yml/extra/event_handler_testsuite.cpp
       ../src_extra/c4/yml/extra/event_handler_testsuite.hpp
       ../src_extra/c4/yml/extra/ints_utils.cpp
       ../src_extra/c4/yml/extra/ints_utils.hpp
       ../src_extra/c4/yml/extra/ints_to_testsuite.cpp
       ../src_extra/c4/yml/extra/ints_to_testsuite.hpp
       ../src_extra/c4/yml/extra/scalar.hpp
       ../src_extra/c4/yml/extra/scalar.cpp
       ../src_extra/c4/yml/extra/string.hpp
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../src_extra
    LIBS ryml c4fs gtest
    FOLDER test)
if(BUILD_SHARED_LIBS)
    # this is needed to resolve get_case() because it is provided by
    # the test linking to this library
    set_target_properties(ryml-_testlib PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
endif()
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


ryml_add_test(engine_1_doc)
ryml_add_test(engine_2_map)
ryml_add_test(engine_3_seq)
ryml_add_test(engine_4_anchor)
ryml_add_test(engine_5_tag)
ryml_add_test(engine_6_qmrk)
ryml_add_test(engine_7_seqimap)
ryml_add_test(engine_8_scalars_tokens)
ryml_add_test(extra_testsuite)
ryml_add_test(extra_ints)
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
# workaround for a false positive warning in gcc14 --std=c++20 -O2
if(CMAKE_COMPILER_IS_GNUCXX
        AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 13)
        AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15))
    get_target_property(std ryml-test-scalar_plain CXX_STANDARD)
    if(std EQUAL 20)
        c4_target_compile_flags(ryml-test-scalar_plain PUBLIC GCC -Wno-array-bounds -Wno-stringop-overflow)
    endif()
endif()


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
        string(REPLACE " " ";" flags "${flags}")
        add_test(NAME ryml-test-tool-events-${name} COMMAND ${RYML_TGT_EVENTS} ${flags} ${file})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-events-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    ryml_get_target_exe(ryml-yaml-events RYML_TGT_EVENTS)
    ryml_add_event_tool_test(success_help TRUE "--help" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(success_help2 TRUE "tst --help" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(fail_nocmd FALSE "" "foo")
    ryml_add_event_tool_test(fail_badcmd FALSE "barbar" "foo")
    ryml_add_event_tool_test(success_timing TRUE "tst --timing" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(success_tree TRUE "tst" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(success_evts TRUE "tss" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(success_evts_ints TRUE "tsi" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(success_ryml_ints TRUE "ri" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(success_evts_ints_resize TRUE "tsi --ints-size 1" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_resize FALSE "tsi --ints-size" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_resize2 FALSE "tsi --ints-size xyz" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_no_resize FALSE "tsi --ints-size 1 --ints-size-force" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_squo_tree FALSE "tst" "foo: 'bar")
    ryml_add_event_tool_test(fail_squo_evts FALSE "tss" "foo: 'bar")
    ryml_add_event_tool_test(fail_dquo_tree FALSE "tst" "foo: \"bar")
    ryml_add_event_tool_test(fail_dquo_evts FALSE "tss" "foo: \"bar")
    ryml_add_event_tool_test(fail_seq1_tree FALSE "tst" "[ a, b, c ] ]")
    ryml_add_event_tool_test(fail_seq2_evts FALSE "tss" "[ [a, b, c ]")
    if(UNIX)
        add_test(NAME ryml-test-tool-events-stdin COMMAND echo foo | ${RYML_TGT_EVENTS} tsi -)
    endif()
    add_test(NAME ryml-test-tool-events-nofile COMMAND ${RYML_TGT_EVENTS} tsi no_file)
    set_tests_properties(ryml-test-tool-events-nofile PROPERTIES WILL_FAIL TRUE)
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
            ../src_extra/c4/yml/extra/event_handler_ints.cpp
            ../src_extra/c4/yml/extra/event_handler_ints.hpp
            ../src_extra/c4/yml/extra/event_handler_testsuite.cpp
            ../src_extra/c4/yml/extra/event_handler_testsuite.hpp
            ../src_extra/c4/yml/extra/ints_utils.cpp
            ../src_extra/c4/yml/extra/ints_utils.hpp
            ../src_extra/c4/yml/extra/ints_to_testsuite.cpp
            ../src_extra/c4/yml/extra/ints_to_testsuite.hpp
            ../src_extra/c4/yml/extra/scalar.cpp
            ../src_extra/c4/yml/extra/scalar.hpp
            ../src_extra/c4/yml/extra/string.hpp
            testsuite.cpp
            testsuite/testsuite_common.hpp
            testsuite/testsuite_events_emitter.cpp
            testsuite/testsuite_events.cpp
            testsuite/testsuite_events.hpp
            testsuite/testsuite_parts.cpp
            testsuite/testsuite_parts.hpp
        LIBS ryml-_testlib c4log
        INC_DIRS ${CMAKE_CURRENT_LIST_DIR}/../src_extra
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
            INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../src_extra
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
            set_tests_properties(ryml-test-fuzz-${name_}-${fuzz_name} PROPERTIES TIMEOUT 10)
        endfunction()
        foreach(fuzz_file ${fuzz_files})
            ryml_add_fuzz_test_file(${name} ${corpus_artifacts_dir} ${fuzz_file})
        endforeach()
    endfunction()
    ryml_add_fuzz_test(parse_emit)
    ryml_add_fuzz_test(events_testsuite
        ../src_extra/c4/yml/extra/event_handler_testsuite.hpp
        ../src_extra/c4/yml/extra/event_handler_testsuite.cpp
        ../src_extra/c4/yml/extra/scalar.hpp
        ../src_extra/c4/yml/extra/scalar.cpp
        ../src_extra/c4/yml/extra/string.hpp)
    ryml_add_fuzz_test(events_ints
        ../src_extra/c4/yml/extra/event_handler_ints.hpp
        ../src_extra/c4/yml/extra/event_handler_ints.cpp
        ../src_extra/c4/yml/extra/scalar.hpp
        ../src_extra/c4/yml/extra/scalar.cpp)
endif()
