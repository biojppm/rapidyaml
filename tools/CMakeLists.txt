
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER tools)

c4_add_executable(ryml-yaml-events
    SOURCES yaml_events.cpp
      ../src_extra/c4/yml/extra/event_handler_test_suite.hpp
      ../src_extra/c4/yml/extra/event_handler_test_suite.cpp
      ../test/test_suite/test_suite_events.hpp
      ../test/test_suite/test_suite_events_emitter.cpp
    INC_DIRS ../test ../src_extra
    LIBS ryml c4fs
    FOLDER tools)
