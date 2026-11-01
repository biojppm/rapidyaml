
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
      ../test/testsuite/testsuite_events.hpp
      ../test/testsuite/testsuite_events_emitter.cpp
    INC_DIRS ../test
    LIBS ryml c4fs
    FOLDER tools)
ryml_testbm_link_with_c4core(ryml-parse-emit)
ryml_maybe_dump_test_yaml(ryml-parse-emit)
