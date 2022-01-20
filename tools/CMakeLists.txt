
c4_add_executable(ryml-parse-emit
    SOURCES parse_emit.cpp
    LIBS ryml c4fs
    FOLDER tools)

c4_add_executable(ryml-yaml-events
    SOURCES yaml_events.cpp
    LIBS ryml c4fs
    FOLDER tools)
