cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart LANGUAGES CXX)

find_package(ryml REQUIRED)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart PRIVATE ryml::ryml)  # note the namespace!

if(WIN32)
    # copy the dll if it is a dll
    get_target_property(ryml_type ryml::ryml TYPE)
    if((ryml_type STREQUAL SHARED_LIBRARY) OR (ryml_type STREQUAL MODULE_LIBRARY))
        get_target_property(ryml_location ryml::ryml LOCATION)
        add_custom_command(TARGET ryml-quickstart POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${ryml_location} $<TARGET_FILE_DIR:ryml-quickstart>)
    endif()
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart> --quiet
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart> --quiet")
