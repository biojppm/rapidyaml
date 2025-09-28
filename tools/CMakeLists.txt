
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER tools)

c4_add_executable(ryml-yaml-events
    SOURCES yaml_events.cpp
      ../src_extra/c4/yml/extra/event_handler_ints.hpp
      ../src_extra/c4/yml/extra/event_handler_ints.cpp
      ../src_extra/c4/yml/extra/event_handler_testsuite.hpp
      ../src_extra/c4/yml/extra/event_handler_testsuite.cpp
      ../src_extra/c4/yml/extra/ints_utils.hpp
      ../src_extra/c4/yml/extra/ints_utils.cpp
      ../src_extra/c4/yml/extra/ints_to_testsuite.hpp
      ../src_extra/c4/yml/extra/ints_to_testsuite.cpp
      ../src_extra/c4/yml/extra/scalar.hpp
      ../src_extra/c4/yml/extra/scalar.cpp
      ../src_extra/c4/yml/extra/string.hpp
      ../test/testsuite/testsuite_events.hpp
      ../test/testsuite/testsuite_events_emitter.cpp
    INC_DIRS ../test ../src_extra
    LIBS ryml c4fs
    FOLDER tools)
