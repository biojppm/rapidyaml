
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER tools)

c4_add_executable(ryml-yaml-events
    SOURCES yaml_events.cpp
      ../test/test_suite/test_suite_events.hpp
      ../test/test_suite/test_suite_events_emitter.cpp
      ../test/test_suite/test_suite_event_handler.hpp
      ../test/test_suite/test_suite_event_handler.cpp
    INC_DIRS ../test
    LIBS ryml c4fs
    FOLDER tools)
