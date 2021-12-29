cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml-quickstart LANGUAGES CXX)

# create a target to amalgamate ryml into a single header:
include(../singleheader/amalgamate.cmake)
amalgamate_ryml(SINGLE_HEADER_DIR SINGLE_HEADER)

# add a library using the amalgamated header
add_library(ryml lib.cpp ${SINGLE_HEADER})
target_compile_features(ryml PUBLIC cxx_std_11)
target_include_directories(ryml PUBLIC "${SINGLE_HEADER_DIR}")

# now simply define the executable:
add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart PRIVATE ryml)
target_compile_definitions(ryml-quickstart PUBLIC -DRYML_SINGLE_HEADER_LIB)

# adjustments for shared library
if(BUILD_SHARED_LIBS)
    # RYML_SHARED should be propagated to targets consuming ryml
    target_compile_definitions(ryml PUBLIC -DRYML_SHARED)
endif()

add_custom_target(run ryml-quickstart
    COMMAND $<TARGET_FILE:ryml-quickstart>
    DEPENDS ryml-quickstart)
