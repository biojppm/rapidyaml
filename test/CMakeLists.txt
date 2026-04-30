c4_setup_testing(GTEST)


function(ryml_add_test_no_lib test_name)
    c4_add_executable(${test_name} LIBS ryml FOLDER test ${ARGN})
    if(RYML_DBG)
        target_compile_definitions(${test_name} PRIVATE RYML_DBG)
        c4_target_compile_flags(${test_name} PUBLIC GCC -Wno-useless-cast)
    endif()
    ryml_maybe_dump_test_yaml(${test_name})
    c4_add_test(${test_name})
endfunction()
ryml_add_test_no_lib(ryml-test-quickstart
    SOURCES ../samples/quickstart.cpp)
ryml_add_test_no_lib(ryml-test-quickstart-ints
    SOURCES
        ../samples/quickstart-ints.cpp
        ../src_extra/c4/yml/extra/event_handler_ints.hpp
        ../src_extra/c4/yml/extra/event_handler_ints.cpp
        ../src_extra/c4/yml/extra/ints_utils.hpp
        ../src_extra/c4/yml/extra/ints_utils.cpp
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
       test_lib/test_save.cpp
       ../src_extra/c4/yml/extra/event_handler_ints.cpp
       ../src_extra/c4/yml/extra/event_handler_ints.hpp
       ../src_extra/c4/yml/extra/ints_utils.cpp
       ../src_extra/c4/yml/extra/ints_utils.hpp
       ../src_extra/c4/yml/extra/ints_to_testsuite.cpp
       ../src_extra/c4/yml/extra/ints_to_testsuite.hpp
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
if(RYML_SAVE_TEST_YAML)
    target_compile_definitions(ryml-_testlib PUBLIC RYML_SAVE_TEST_YAML)
endif()

c4_add_library(ryml-_testmain LIBRARY_TYPE OBJECT
    SOURCES test_lib/test_main.cpp test_lib/test_main.hpp
    LIBS ryml
    FOLDER test
)
if(DEFINED RYML_DEFAULT_CALLBACKS)
    if(NOT RYML_DEFAULT_CALLBACKS)
        target_compile_definitions(ryml-_testmain PUBLIC RYML_NO_DEFAULT_CALLBACKS)
    endif()
endif()

function(ryml_add_test test_name)
    cmake_parse_arguments("" "BIG" "" "LIBS" ${ARGN})
    set(t ryml-test-${test_name})
    c4_add_executable(${t}
        SOURCES test_${test_name}.cpp
        LIBS ryml-_testlib ryml-_testmain
        FOLDER test)
    if(RYML_SAVE_TEST_YAML)
        target_sources(${t} PRIVATE test_lib/test_save.cpp)
    endif()
    c4_add_test(${t})
    if(MSVC AND _BIG) # MSVC bigobj
        target_compile_options(${t} PRIVATE /bigobj)
    endif()
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
    ryml_add_test(${name} ${ARGN})
    target_link_libraries(ryml-test-${name} PUBLIC ryml-_testgroup)
endfunction()


ryml_add_test(engine_1_doc_1 BIG)
ryml_add_test(engine_1_doc_2 BIG)
ryml_add_test(engine_2_seq_block_1 BIG)
ryml_add_test(engine_2_seq_block_2 BIG)
ryml_add_test(engine_2_seq_flow_1 BIG)
ryml_add_test(engine_2_seq_flow_2 BIG)
ryml_add_test(engine_2_seq_flow_3 BIG)
ryml_add_test(engine_3_map_block_1 BIG)
ryml_add_test(engine_3_map_block_2 BIG)
ryml_add_test(engine_3_map_flow_1 BIG)
ryml_add_test(engine_3_map_flow_2 BIG)
ryml_add_test(engine_3_map_flow_3 BIG)
ryml_add_test(engine_4_anchor BIG)
ryml_add_test(engine_5_tag_1 BIG)
ryml_add_test(engine_5_tag_2 BIG)
ryml_add_test(engine_5_tag_3 BIG)
ryml_add_test(engine_6_qmrk_1 BIG)
ryml_add_test(engine_6_qmrk_2 BIG)
ryml_add_test(engine_6_qmrk_3 BIG)
ryml_add_test(engine_6_qmrk_4 BIG)
ryml_add_test(engine_7_seqimap)
ryml_add_test(engine_8_scalars_tokens)
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
ryml_add_test(bom_1)
ryml_add_test(bom_2)
ryml_add_test(escape_scalar)
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
ryml_add_test_case_group(tag)
ryml_add_test_case_group(qmrk)
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
    function(ryml_add_event_tool_test_ name expect_success args contents)
        string(REPLACE " " ";" args "${args}")
        add_test(NAME ryml-test-tool-events-${name} COMMAND ${RYML_TGT_EVENTS} ${args} ${file})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-events-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    function(ryml_add_event_tool_test name expect_success args contents)
        ryml_create_file(${name}.yml "${contents}" file)
        ryml_add_event_tool_test_(${name} ${expect_success} "${args}" "${contents}")
    endfunction()
    function(ryml_add_event_tool_test_all_ name expect_success args contents with_tree)
        ryml_create_file(${name}.yml "${contents}" file)
        if(${with_tree})
            ryml_add_event_tool_test_(${name}-tree ${expect_success} "tst ${args}" "${contents}")
        endif()
        ryml_add_event_tool_test_(${name}-ints ${expect_success} "tsi ${args}" "${contents}")
        ryml_add_event_tool_test_(${name}-rymlints ${expect_success} "ri ${args}" "${contents}")
    endfunction()
    function(ryml_add_event_tool_test_all name expect_success args contents)
        ryml_add_event_tool_test_all_(${name} ${expect_success} "${args}" "${contents}" TRUE)
    endfunction()
    function(ryml_add_event_tool_test_tsonly name expect_success args contents)
        ryml_add_event_tool_test_all_(${name} ${expect_success} "${args}" "${contents}" FALSE)
    endfunction()
    ryml_get_target_exe(ryml-yaml-events RYML_TGT_EVENTS)
    ryml_add_event_tool_test(success_help TRUE "--help" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_help2 TRUE "--help" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test(fail_nocmd FALSE "" "foo")
    ryml_add_event_tool_test(fail_badcmd FALSE "barbar" "foo")
    ryml_add_event_tool_test_all(success TRUE "" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_timing TRUE " --timing" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_quiet TRUE " --quiet" "{foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_tsonly(success_contkeys TRUE "" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(success_evts_ints_resize TRUE "tsi --ints-size 1" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_resize FALSE "tsi --ints-size" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_resize2 FALSE "tsi --ints-size xyz" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test(fail_evts_ints_no_resize FALSE "tsi --ints-size 1 --ints-size-force" "{{this: is, a: keymap}: [seq,val]}")
    ryml_add_event_tool_test_all(fail_squo FALSE "" "foo: 'bar")
    ryml_add_event_tool_test_all(fail_dquo FALSE "" "foo: \"bar")
    ryml_add_event_tool_test_all(fail_seq1 FALSE "" "[ [a, b, c ]")
    ryml_add_event_tool_test_all(fail_seq2 FALSE "" "[ a, b, c ] ]")
    ryml_add_event_tool_test_all(success_custom_tag1 TRUE "  "                "{ !footag foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_custom_tag2 TRUE " --resolve-tags "  "{ !footag foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_noresolvetags TRUE ""                "{ !m!tag foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(fail_unresolved_tag FALSE " --resolve-tags " "{ !m!tag foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(fail_unresolved_tag_short FALSE " -rt "      "{ !m!tag foo: bar, baz: [exactly]}")
    ryml_add_event_tool_test_all(success_resolvetags TRUE " --resolve-tags"   "%TAG !m! my-\n---\n{ !m!tag foo: bar, baz: [exactly]}")
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
            ../src_extra/c4/yml/extra/ints_utils.cpp
            ../src_extra/c4/yml/extra/ints_utils.hpp
            ../src_extra/c4/yml/extra/ints_to_testsuite.cpp
            ../src_extra/c4/yml/extra/ints_to_testsuite.hpp
            ../src_extra/c4/yml/extra/string.hpp
            testsuite.cpp
            testsuite/testsuite_common.hpp
            testsuite/testsuite_events_emitter.cpp
            testsuite/testsuite_events.cpp
            testsuite/testsuite_events.hpp
            testsuite/testsuite_parts.cpp
            testsuite/testsuite_parts.hpp
        LIBS ryml-_testlib ryml-_testmain c4log
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
            LIBS ryml ryml-_testmain c4fs
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
    ryml_add_fuzz_test(events_ints
        ../src_extra/c4/yml/extra/event_handler_ints.hpp
        ../src_extra/c4/yml/extra/event_handler_ints.cpp)
endif()
