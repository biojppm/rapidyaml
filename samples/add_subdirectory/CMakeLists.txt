cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart LANGUAGES CXX)

add_subdirectory(../../ ryml)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml::ryml)
if(BUILD_SHARED_LIBS)
    # simplify things by placing the exe next to the dll
    set_target_properties(ryml-quickstart PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:ryml>)
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart> --quiet
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart> --quiet")
