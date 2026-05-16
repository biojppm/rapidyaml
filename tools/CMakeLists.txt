
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER tools)
ryml_maybe_dump_test_yaml(ryml-parse-emit)

c4_add_executable(ryml-yaml-events
    SOURCES yaml_events.cpp
      ../test/testsuite/testsuite_events.hpp
      ../test/testsuite/testsuite_events_emitter.cpp
    INC_DIRS ../test
    LIBS ryml c4fs
    FOLDER tools)
ryml_maybe_dump_test_yaml(ryml-yaml-events)
