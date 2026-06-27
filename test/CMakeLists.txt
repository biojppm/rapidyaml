c4_setup_testing(GTEST)

function(ryml_get_target_exe target_name target_file)
    if(CMAKE_CROSSCOMPILING)
        set(tgt ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${target_name}>)
    else()
        set(tgt $<TARGET_FILE:${target_name}>)
    endif()
    set(${target_file} ${tgt} PARENT_SCOPE)
endfunction()

function(ryml_setup_test_target_props target_name)
    c4_target_compile_flags(${target_name} PUBLIC GCC -Wno-useless-cast)
    if(RYML_DBG)
        target_compile_definitions(${target_name} PUBLIC RYML_DBG)
    endif()
    if(DEFINED RYML_DEFAULT_CALLBACKS)
        if(NOT RYML_DEFAULT_CALLBACKS)
            target_compile_definitions(${target_name} PUBLIC RYML_NO_DEFAULT_CALLBACKS)
        endif()
    endif()
    if(RYML_SAVE_TEST_YAML)
        target_compile_definitions(${target_name} PUBLIC RYML_SAVE_TEST_YAML)
    endif()
    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY")
        set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif ()
endfunction()

function(ryml_add_test_exe_no_lib name)
    c4_add_executable(${name} LIBS ryml FOLDER test ${ARGN})
    ryml_setup_test_target_props(${name})
    ryml_maybe_dump_test_yaml(${name})
    add_dependencies(ryml-test-build ${name})
endfunction()

function(ryml_add_test_no_lib test_name)
    ryml_add_test_exe_no_lib(${test_name} ${ARGN})
    c4_add_test(${test_name})
endfunction()


# quickstart tests
ryml_add_test_exe_no_lib(ryml-test-quickstart  SOURCES ../samples/quickstart.cpp)
ryml_add_test_exe_no_lib(ryml-test-quickstart-ints SOURCES ../samples/quickstart-ints.cpp)
function(ryml_add_quickstart_test targetname suffix)
    set(testname ${targetname}-${suffix})
    cmake_parse_arguments("" "STDIN;LOCK;WILL_FAIL" "" "ARGS" ${ARGN})
    ryml_get_target_exe(${targetname} exe)
    # c4_add_test() cannot be used with multiple tests from the same
    # exe, so we use explicit add_test()+valgrind. Also, use
    # RESOURCE_LOCK to prevent the tests from running simultaneously,
    # because they write/read to the same file.
    if(NOT _STDIN)
        add_test(NAME ${testname} COMMAND ${CMAKE_COMMAND} -E env ${exe} ${_ARGS})
    else()
        if(NOT UNIX)
            return()
        endif()
        add_test(NAME ${testname} COMMAND ${CMAKE_COMMAND} -E env echo "foo: bar" | ${exe} ${_ARGS})
    endif()
    if(_LOCK)
        set(lockid ${targetname}-lock)
        set_tests_properties(${testname} PROPERTIES RESOURCE_LOCK ${lockid})
    endif()
    if(_WILL_FAIL)
        set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
    endif()
    if(RYML_VALGRIND)
        separate_arguments(_vg_opts UNIX_COMMAND "${RYML_VALGRIND_OPTIONS}")
        add_test(NAME ${testname}-valgrind
            COMMAND valgrind ${_vg_opts} $<TARGET_FILE:${targetname}> ${_ARGS}
            COMMAND_EXPAND_LISTS)
        if(_LOCK)
            set_tests_properties(${testname}-valgrind PROPERTIES RESOURCE_LOCK ${lockid})
        endif()
        if(_WILL_FAIL)
            set_tests_properties(${testname}-valgrind PROPERTIES WILL_FAIL TRUE)
        endif()
    endif()
endfunction()
ryml_add_quickstart_test(ryml-test-quickstart noargs LOCK)
ryml_add_quickstart_test(ryml-test-quickstart quiet ARGS --quiet LOCK)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-help ARGS --help)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-quiet ARGS --quiet)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-zero ARGS -e 0 -a 0)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-stdin STDIN ARGS -)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-stdin2 STDIN ARGS stdin)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-noretry ARGS -n)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-noretry-fail WILL_FAIL ARGS -n -e 0)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-file ARGS ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-too-many WILL_FAIL ARGS a b c)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-missing_events WILL_FAIL ARGS -e)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-missing_arena WILL_FAIL ARGS -a)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-bad_events WILL_FAIL ARGS -e bad)
ryml_add_quickstart_test(ryml-test-quickstart-ints  ints-bad_arena WILL_FAIL ARGS -a bad)


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
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../src_extra
    LIBS ryml c4fs gtest
    FOLDER test)
ryml_setup_test_target_props(ryml-_testlib)
ryml_add_c4core_dev_to_target(ryml-_testlib)
c4_add_library(ryml-_testmain LIBRARY_TYPE OBJECT
    SOURCES test_lib/test_main.cpp test_lib/test_main.hpp
    LIBS ryml
    FOLDER test
)

function(ryml_add_test test_name)
    cmake_parse_arguments("" "BIG" "" "LIBS" ${ARGN})
    set(t ryml-test-${test_name})
    c4_add_executable(${t}
        SOURCES test_${test_name}.cpp
        LIBS ryml-_testlib ryml-_testmain
        FOLDER test)
    ryml_add_c4core_dev_to_target(${t})
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
ryml_setup_test_target_props(ryml-_testgroup)
ryml_add_c4core_dev_to_target(ryml-_testgroup)
function(ryml_add_test_case_group name)
    ryml_add_test(${name} ${ARGN})
    target_link_libraries(ryml-test-${name} PUBLIC ryml-_testgroup)
endfunction()


# some subjects have to be split over several files because gtest
# causes some amount of bloat and some compilers (VS) or platforms
# (mips) choke on the large binary

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
ryml_add_test(engine_7_seqimap BIG)
ryml_add_test(engine_8_scalars_tokens BIG)
ryml_add_test(engine_9_indentation BIG)
ryml_add_test(extra_ints)
ryml_add_test(version)
ryml_add_test(callbacks)
ryml_add_test(stack)
ryml_add_test(filter)
ryml_add_test(parser)
ryml_add_test(node_type)
ryml_add_test(tree)
ryml_add_test(tree_build)
ryml_add_test(noderef)
ryml_add_test(emit)
ryml_add_test(style)
ryml_add_test(serialize_1)
ryml_add_test(serialize_2)
ryml_add_test(serialize_3)
ryml_add_test(basic)
ryml_add_test(json)
ryml_add_test(merge)
ryml_add_test(location)
ryml_add_test(bom_1)
ryml_add_test(bom_2)
ryml_add_test(escape_scalar)
ryml_add_test(print_tree)
ryml_add_test(file)
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
# test errors on default callbacks (calls abort)

if(RYML_DEFAULT_CALLBACKS AND NOT WIN32) # windows does like the abort call
    function(ryml_add_test_error name)
        set(tgt ryml-test-error-${name})
        set(src test_error_${name}.cpp)
        add_executable(${tgt} ${src} test_error_dump_gcov_on_error.hpp)
        add_dependencies(ryml-test-build ${tgt})
        set_target_properties(${tgt} PROPERTIES FOLDER test)
        target_link_libraries(${tgt} PRIVATE ryml)
        # ensure lcov data is dumped on abort
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
            target_compile_definitions(${tgt} PRIVATE RYML_COVERAGE)
        endif()
        # get the exe to run the test
        ryml_get_target_exe(${tgt} exe)
        # when abort() is called, cmake fails the test even if
        # WILL_FAIL is set to true for it. To be able to pass the
        # test, it has to be wrapped in a cmake command to run the
        # executable. See the cmake help for WILL_FAIL.
        add_test(NAME ${tgt} COMMAND ${CMAKE_COMMAND} -E env ${exe})
        set_tests_properties(${tgt} PROPERTIES WILL_FAIL TRUE)
    endfunction()
    ryml_add_test_error(basic)
    ryml_add_test_error(parse)
    ryml_add_test_error(visit)
endif()


#-------------------------------------------------------------------------
# test the tools as well

if(NOT EMSCRIPTEN)
    option(RYML_TEST_TOOLS "Enable tests for the tools. Requires file system access." ON)
endif()

if(RYML_TEST_TOOLS)
    if(NOT RYML_BUILD_TOOLS)
        add_subdirectory(../tools tools)
    endif()
    add_dependencies(ryml-test-build ryml-parse-emit)
    add_dependencies(ryml-test-build ryml-yaml-events)
    function(ryml_create_file name contents subdir fileout basenameout)
        set(dir ${CMAKE_CURRENT_BINARY_DIR}/${subdir})
        if(NOT EXISTS ${dir})
            file(MAKE_DIRECTORY ${dir})
        endif()
        set(filename ${dir}/${name})
        file(WRITE "${filename}" "${contents}
")
        set("${fileout}" "${filename}" PARENT_SCOPE)
        set("${basenameout}" "${name}" PARENT_SCOPE)
    endfunction()
    #
    # parse & emit
    if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
        c4_err("could not find test file")
    endif()
    ryml_get_target_exe(ryml-parse-emit RYML_TGT_PARSE_EMIT)
    set(parseemit_dir testfiles-parse_emit)
    function(ryml_add_parse_emit_tool_test_ name expect_success args file)
        string(REPLACE " " ";" args "${args}")
        add_test(NAME ryml-test-tool-parse_emit-${name}
            COMMAND ${CMAKE_COMMAND} -E env ${RYML_TGT_PARSE_EMIT} ${args} ${file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${parseemit_dir})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-parse_emit-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    function(ryml_add_parse_emit_tool_test name expect_success args contents)
        ryml_create_file(${name}.yml "${contents}" ${parseemit_dir} file basename)
        ryml_add_parse_emit_tool_test_(${name} ${expect_success} "${args}" ${basename})
    endfunction()
    if(UNIX)
        add_test(NAME ryml-test-tool-parse_emit-stdin COMMAND echo foo | ${RYML_TGT_PARSE_EMIT} -t -p -)
    endif()
    add_test(NAME ryml-test-tool-parse_emit-appveyor COMMAND ${RYML_TGT_PARSE_EMIT} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml)
    set(yaml "[abc,a,b,c]")
    ryml_add_parse_emit_tool_test_(args_none         FALSE "" "")
    ryml_add_parse_emit_tool_test_(args_help        TRUE "--help" "")
    ryml_add_parse_emit_tool_test(args_reserve      TRUE "-e 10" "${yaml}")
    ryml_add_parse_emit_tool_test(args_reserve_long TRUE "--reserve 10" "${yaml}")
    ryml_add_parse_emit_tool_test(args_reserve_estimate TRUE "-e 1" "${yaml}")
    ryml_add_parse_emit_tool_test(args_reserve_noval FALSE "--reserve" "${yaml}")
    ryml_add_parse_emit_tool_test(args_reserve_badval FALSE "--reserve not-int" "${yaml}")
    ryml_add_parse_emit_tool_test(args_resolve      TRUE "-r" "${yaml}")
    ryml_add_parse_emit_tool_test(args_resolve_long TRUE "--resolve" "${yaml}")
    ryml_add_parse_emit_tool_test(args_resolve_keep TRUE "-r -k" "${yaml}")
    ryml_add_parse_emit_tool_test(args_resolve_keep_long TRUE "--resolve --keep-refs" "${yaml}")
    ryml_add_parse_emit_tool_test(args_quiet_long   TRUE "-q" "${yaml}")
    ryml_add_parse_emit_tool_test(args_print_tree   TRUE "-p" "${yaml}")
    ryml_add_parse_emit_tool_test(args_print_tree_long TRUE "--print-tree" "${yaml}")
    ryml_add_parse_emit_tool_test(args_quiet        TRUE "--quiet" "${yaml}")
    ryml_add_parse_emit_tool_test(args_json         TRUE "-j" "${yaml}")
    ryml_add_parse_emit_tool_test(args_json_long    TRUE "--json" "${yaml}")
    ryml_add_parse_emit_tool_test(args_string       TRUE "-s" "${yaml}")
    ryml_add_parse_emit_tool_test(args_string_long  TRUE "--string" "${yaml}")
    ryml_add_parse_emit_tool_test(args_timed        TRUE "-t" "${yaml}")
    ryml_add_parse_emit_tool_test(args_timed_long   TRUE "--timed" "${yaml}")
    ryml_add_parse_emit_tool_test(args_output       TRUE "-o peoutput" "${yaml}")
    ryml_add_parse_emit_tool_test(args_output_long  TRUE "--output peoutput_long" "${yaml}")
    ryml_add_parse_emit_tool_test(args_output_noval FALSE "-o" "${yaml}")
    ryml_add_parse_emit_tool_test(args_output_invalid FALSE "-o invalid/dir/file.out" "${yaml}")
    ryml_add_parse_emit_tool_test(args_unknown      FALSE "--asdlkjsadlj file" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_yaml     TRUE "-e 1 -t -p -r" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_json     TRUE "-e 1 -t -p -r -j" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_yaml_str TRUE "-e 1 -t -p -s" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_json_str TRUE "-e 1 -t -p -s -j" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_yaml_out TRUE "-e 1 -t -p -o outyaml.tpe" "${yaml}")
    ryml_add_parse_emit_tool_test(args_all_json_out TRUE "-e 1 -t -p -o outjson.tpe -j" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints         TRUE "-i" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_long    TRUE "--ints" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_noreserve TRUE "--ints -e 0" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_estimate TRUE "--ints -e 1" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_reserve_fixed TRUE "--ints -e 2" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_reserve_large TRUE "--ints -e 30" "${yaml}")
    ryml_add_parse_emit_tool_test(args_ints_all     TRUE "--ints -e 30 -p -t" "${yaml}")
    ryml_add_parse_emit_tool_test(error_parse       FALSE "" "{foo: bar")
    ryml_add_parse_emit_tool_test(error_visit       FALSE "--resolve" "[&a b,*c]")
    #
    # events emitter
    set(events_dir testfiles-events)
    function(ryml_add_event_tool_test_ name expect_success args file)
        string(REPLACE " " ";" args "${args}")
        add_test(NAME ryml-test-tool-events-${name}
            COMMAND ${CMAKE_COMMAND} -E env ${RYML_TGT_EVENTS} ${args} ${file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${events_dir})
        if(NOT expect_success)
            set_tests_properties(ryml-test-tool-events-${name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endfunction()
    function(ryml_add_event_tool_test name expect_success args contents)
        ryml_create_file(${name}.yml "${contents}" ${events_dir} file basename)
        ryml_add_event_tool_test_(${name} ${expect_success} "${args}" ${basename})
    endfunction()
    function(ryml_add_event_tool_test_all_ name expect_success args contents with_tree)
        ryml_create_file(${name}.yml "${contents}" ${events_dir} file basename)
        if(${with_tree})
            ryml_add_event_tool_test_(${name}-tree ${expect_success} "tst ${args}" ${basename})
        endif()
        ryml_add_event_tool_test_(${name}-ints ${expect_success} "tsi ${args}" ${basename})
        ryml_add_event_tool_test_(${name}-rymlints ${expect_success} "ri ${args}" ${basename})
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

    set(tsdir ${ed}/yaml-test-suite)
    c4_download_remote_proj(yaml-test-suite suite_dir
        GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
        GIT_TAG data-2022-01-17)
    if(NOT EXISTS ${suite_dir}/229Q)
        c4_err("cannot find yaml-test-suite at ${suite_dir} -- was there an error downloading the project?")
    endif()

    c4_add_executable(ryml-test-suite
        SOURCES
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
    ryml_add_c4core_dev_to_target(ryml-test-suite)
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

option(RYML_FUZZ_TEST "Add tests for known fuzz failure cases." ${fuzzdefault})
if(RYML_FUZZ_TEST)
    option(RYML_FUZZ_TEST_INDIVIDUAL "Run one unit test per fuzz case (uses many tests)." OFF)
endif()


if(RYML_FUZZ_TEST OR RYML_FUZZ_DRIVERS)
    if(RYML_FUZZ_DRIVERS)
        set(ryml_fuzz_umbrella_cmds)
    endif()
    function(ryml_add_fuzz_variant name fuzz_mk_target fuzzdir faildir)
        function(ryml_add_fuzz_exe exename)
            cmake_parse_arguments("" "" "" "SRC;LIBS" ${ARGN})
            c4_add_executable(${exename}
                SOURCES ${_SRC}
                INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../src_extra
                LIBS ryml ${_LIBS}
                FOLDER test/fuzz)
            if(RYML_DBG)
                target_compile_definitions(${exename} PUBLIC RYML_DBG)
            endif()
        endfunction()
        #
        # add unit test
        #
        ryml_add_fuzz_exe(ryml-test-fuzz-${name}
            SRC test_fuzz/test_fuzz_main.cpp
                test_fuzz/test_fuzz_${name}.cpp
                test_fuzz/test_fuzz_common.hpp
            LIBS ryml-_testmain c4fs
        )
        add_dependencies(ryml-test-build ryml-test-fuzz-${name})
        ryml_get_target_exe(ryml-test-fuzz-${name} testexe)
        if(RYML_FUZZ_TEST_INDIVIDUAL)
            # run tests individually. this would create many thousands of
            # unit tests, so we only run the fail dir, which has ~1000
            file(GLOB_RECURSE fuzz_cases RELATIVE "${fuzzdir}/${faildir}" "${fuzzdir}/${faildir}/*")
            foreach(file ${fuzz_cases})
                string(REPLACE "/" "_" fuzz_name "${file}")
                add_test(NAME ryml-test-fuzz-${name}-${fuzz_name}
                    COMMAND ${testexe} ${fuzzdir}/${faildir}/${file})
                set_tests_properties(ryml-test-fuzz-${name}-${fuzz_name} PROPERTIES TIMEOUT 10)
            endforeach()
        else()
            # pass the directory as argument to the test exe, which
            # will then iterate over the files. this generates only a
            # handful of unit tests, so we can run everything.
            foreach(corpus_dir ${faildir} ${ARGN})
                add_test(NAME ryml-test-fuzz-${name}-${corpus_dir}
                    COMMAND ${testexe} ${fuzzdir}/${corpus_dir}
                )
                set_tests_properties(ryml-test-fuzz-${name}-${corpus_dir} PROPERTIES TIMEOUT 1000)
            endforeach()
        endif()
        #
        # add a fuzz driver?
        #
        if(RYML_FUZZ_DRIVERS)
            if(NOT CMAKE_CXX_COMPILER_ID STREQUAL Clang)
                message(FATAL_ERROR "Fuzz binaries require Clang. id=${CMAKE_CXX_COMPILER_ID}")
            endif()
            # add the executable
            ryml_add_fuzz_exe(ryml-fuzz-${name}
                SRC test_fuzz/test_fuzz_${name}.cpp
                    test_fuzz/test_fuzz_common.hpp
            )
            if(NOT TARGET ryml-fuzz-drivers-build)
                add_custom_target(ryml-fuzz-drivers-build)
            endif()
            add_dependencies(ryml-fuzz-drivers-build ryml-fuzz-${name})
            # copy the executable to fuzzdir/bin
            add_custom_command(TARGET ryml-fuzz-${name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     $<TARGET_FILE:ryml-fuzz-${name}>
                     ${fuzzdir}/bin/ryml-fuzz-${name}
                COMMENT "${name}: placing at ${fuzzdir}/bin/ryml-fuzz-${name}"
            )
            # this is the fuzz command.
            # it is better to run by hand (see the options in the
            # makefile), but this command is enough to get going:
            set(cmd
                # use the makefile
                ${CMAKE_MAKE_PROGRAM} -C ${fuzzdir}
                ${fuzz_mk_target}
                FUZZBIN=bin/ryml-fuzz-${name}
                ${RYML_FUZZ_ARGS}
            )
            string(REPLACE ";" " " cmd_spc "${cmd}")
            # add the fuzz run target
            add_custom_target(ryml-fuzz-${name}-run
                COMMAND ${CMAKE_COMMAND} -E echo ${cmd_spc}
                COMMAND ${cmd}
                COMMENT "${cmd_spc}"
                DEPENDS ryml-fuzz-${name}
            )
            # append to the umbrella list
            set(all ${ryml_fuzz_umbrella_cmds})
            list(APPEND all
                COMMAND ${CMAKE_COMMAND}
                    --build ${CMAKE_BINARY_DIR}
                    --target ryml-fuzz-${name}-run
            )
            set(ryml_fuzz_umbrella_cmds ${all} PARENT_SCOPE)
        endif()
    endfunction()

    # get the fuzz repo
    c4_download_remote_proj(rapidyaml-data rapidyaml_data_dir
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml-data
        GIT_TAG 20260621)
    if(NOT EXISTS ${rapidyaml_data_dir}/fuzz/yaml.dict)
        message(FATAL_ERROR "cannot find rapidyaml-data at ${rapidyaml_data_dir} -- was there an error downloading the project?")
    endif()
    # add all the fuzzes
    if(RYML_FUZZ_TEST_INDIVIDUAL)
        set(srlz_dirs srlz_fails)
        set(json_dirs json_fails)
        set(yaml_dirs yaml_fails ${json_dirs})
    else()
        set(srlz_dirs srlz_fails srlz_merged)
        set(json_dirs json_fails json_merged json_rapidyamldump)
        set(yaml_dirs yaml_fails yaml_merged yaml_rapidyamldump yaml_test_suite
                      ${json_dirs})
    endif()
    ryml_add_fuzz_variant(srlz      srlz ${rapidyaml_data_dir}/fuzz ${srlz_dirs})
    ryml_add_fuzz_variant(yaml_tree yaml ${rapidyaml_data_dir}/fuzz ${yaml_dirs})
    ryml_add_fuzz_variant(yaml_ints yaml ${rapidyaml_data_dir}/fuzz ${yaml_dirs})
    ryml_add_fuzz_variant(json_tree json ${rapidyaml_data_dir}/fuzz ${json_dirs})
    ryml_add_fuzz_variant(json_ints json ${rapidyaml_data_dir}/fuzz ${json_dirs})
    # umbrella runner
    if(RYML_FUZZ_DRIVERS)
        add_custom_target(ryml-fuzz-drivers-run
            ${ryml_fuzz_umbrella_cmds}
            DEPENDS ryml-fuzz-drivers-build
        )
    endif()
endif()
