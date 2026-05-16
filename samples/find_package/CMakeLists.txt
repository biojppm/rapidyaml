cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart LANGUAGES CXX)

find_package(ryml REQUIRED)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml::ryml)  # note the namespace!

add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart> --quiet
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart> --quiet")
