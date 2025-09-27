cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml-quickstart-ints LANGUAGES CXX)

# create a target to amalgamate ryml into a single header:
include(amalgamate.cmake)
amalgamate_ryml(SINGLE_HEADER_DIR SINGLE_HEADER)

# now simply define the executable:
add_executable(ryml-quickstart-ints ../quickstart-ints.cpp ${SINGLE_HEADER})
target_compile_features(ryml-quickstart-ints PUBLIC cxx_std_11)
target_compile_definitions(ryml-quickstart-ints PUBLIC -DRYML_SINGLE_HEADER)
target_compile_definitions(ryml-quickstart-ints PUBLIC -DRYML_SINGLE_HEADER_INTS)
target_include_directories(ryml-quickstart-ints PUBLIC "${SINGLE_HEADER_DIR}")

add_custom_target(run ryml-quickstart-ints
    COMMAND $<TARGET_FILE:ryml-quickstart-ints>
    DEPENDS ryml-quickstart-ints)
