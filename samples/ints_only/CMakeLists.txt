cmake_minimum_required(VERSION 3.12)
project(quickstart-ints_only
    DESCRIPTION "shows how to build a minimal library containing only the integer events"
    HOMEPAGE_URL ""
    LANGUAGES CXX)

set(RAPIDYAML_DIR ../..)
set(C4CORE_DIR ${RAPIDYAML_DIR}/ext/c4core)

# create a mini-library containing only the integer parsing
# no tree.
add_library(rapidyaml-ints
    #
    # files required from rapidyaml
    ${RAPIDYAML_DIR}/src/c4/yml/error.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/error.def.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/common.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/common.cpp
    ${RAPIDYAML_DIR}/src/c4/yml/node_type.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/node_type.cpp
    ${RAPIDYAML_DIR}/src/c4/yml/parse_engine.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/parse_engine.def.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/tag.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/tag.cpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/event_handler_ints.hpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/event_handler_ints.cpp
    #
    # files required from c4core
    ${C4CORE_DIR}/src/c4/substr.hpp
    ${C4CORE_DIR}/src/c4/error.hpp
    ${C4CORE_DIR}/src/c4/error.cpp
    ${C4CORE_DIR}/src/c4/language.hpp
    ${C4CORE_DIR}/src/c4/language.cpp
    ${C4CORE_DIR}/src/c4/utf.hpp
    ${C4CORE_DIR}/src/c4/utf.cpp
)
target_include_directories(rapidyaml-ints PUBLIC
    ${RAPIDYAML_DIR}/src
    ${RAPIDYAML_DIR}/src_extra
    ${C4CORE_DIR}/src
)

# now create the executable and link with that library. Also, the
# library does not need the ints utils, but the exe does, so we add
# those files directly here.
add_executable(quickstart-ints
    ../quickstart-ints.cpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/ints_utils.hpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/ints_utils.cpp
)
target_link_libraries(quickstart-ints rapidyaml-ints)


add_custom_target(run
    DEPENDS quickstart-ints
    COMMAND $<TARGET_FILE:quickstart-ints>
    COMMENT "running quickstart-ints"
)
