cmake_minimum_required(VERSION 3.12)
project(quickstart-ints_only
    DESCRIPTION "shows how to build a minimal library containing only the integer events"
    HOMEPAGE_URL ""
    LANGUAGES CXX)


# these are the directories from where to read the source files
set(RAPIDYAML_DIR ../..)
set(C4CORE_DIR ${RAPIDYAML_DIR}/ext/c4core)


# create a mini-library with only the integer parsing,
# and without any tree facilities.
add_library(ryml-ints
    #
    # files required from rapidyaml
    ${RAPIDYAML_DIR}/src/c4/yml/error.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/error.def.hpp
    ${RAPIDYAML_DIR}/src/c4/yml/export.hpp
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
target_compile_features(ryml-ints PUBLIC cxx_std_11)
target_include_directories(ryml-ints PUBLIC
    ${RAPIDYAML_DIR}/src
    ${RAPIDYAML_DIR}/src_extra
    ${C4CORE_DIR}/src
)


# now create the executable and link with that library. Also, the
# library does not need the ints utils, but the exe does, so we add
# the util source files directly to the executable.
add_executable(ryml-quickstart-ints
    ../quickstart-ints.cpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/ints_utils.hpp
    ${RAPIDYAML_DIR}/src_extra/c4/yml/extra/ints_utils.cpp
)
target_link_libraries(ryml-quickstart-ints ryml-ints)


# adjustments for shared library
if(WIN32 AND BUILD_SHARED_LIBS)
    # RYML_SHARED should be propagated to targets consuming ryml
    target_compile_definitions(ryml-ints PUBLIC -DRYML_SHARED -DRYML_EXPORTS)
    target_compile_definitions(ryml-ints PUBLIC -DC4CORE_SHARED -DC4CORE_EXPORTS)
    # simplify things by placing the exe next to the dll
    set_target_properties(ryml-quickstart-ints PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:ryml-ints>)
endif()


add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart-ints>
    DEPENDS ryml-quickstart-ints
    COMMENT "running $<TARGET_FILE:ryml-quickstart-ints>"
)
