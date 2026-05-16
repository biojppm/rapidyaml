cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(ryml-quickstart LANGUAGES CXX)

set(RYML_REPO_URL https://github.com/biojppm/rapidyaml CACHE STRING "")
set(RYML_BRANCH_NAME master CACHE STRING "")
message(STATUS "FetchContent from repo: ${RYML_REPO_URL}")
message(STATUS "FetchContent from branch: ${RYML_BRANCH_NAME}")
include(FetchContent)
FetchContent_Declare(ryml
    GIT_REPOSITORY ${RYML_REPO_URL}
    GIT_TAG ${RYML_BRANCH_NAME}
    GIT_SHALLOW FALSE  # ensure submodules are checked out
)
FetchContent_MakeAvailable(ryml)

add_executable(ryml-quickstart ../quickstart.cpp)
target_link_libraries(ryml-quickstart ryml::ryml)

if(WIN32)
    function(maybe_copy_dll_to_exe_dir exe lib)
        if(TARGET ${lib})
            get_target_property(type ${lib} TYPE)
            if((type STREQUAL SHARED_LIBRARY) OR (type STREQUAL MODULE_LIBRARY))
                add_custom_command(TARGET ${exe} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${exe}>)
            endif()
        endif()
    endfunction()
    maybe_copy_dll_to_exe_dir(ryml-quickstart ryml::ryml)
    maybe_copy_dll_to_exe_dir(ryml-quickstart c4core::c4core)
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:ryml-quickstart> --quiet
    DEPENDS ryml-quickstart
    COMMENT "running: $<TARGET_FILE:ryml-quickstart> --quiet")
