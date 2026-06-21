cmake_minimum_required(VERSION 3.12)
project(ryml-quickstart
    DESCRIPTION "Shows how to use a custom c4core version different from ryml's c4core submodule"
    LANGUAGES CXX)
include(../../ext/c4core/cmake/c4Project.cmake)
c4_project(VERSION 0.1.0 STANDALONE
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

set(C4CORE_REPO https://github.com/biojppm/c4core CACHE STRING "")
set(C4CORE_TAG master CACHE STRING "")
include(FetchContent)
message(STATUS "fetch c4core from repo: ${C4CORE_REPO}")
message(STATUS "fetch c4core from tag: ${C4CORE_TAG}")
FetchContent_Declare(c4core
    GIT_REPOSITORY ${C4CORE_REPO}
    GIT_TAG ${C4CORE_TAG}
)
FetchContent_MakeAvailable(c4core)

# and now use RYML_STANDALONE=OFF to inform ryml to link with c4core
# (which must already be available as a target), instead of using the
# c4core sources packaged with ryml:
set(RYML_STANDALONE OFF CACHE BOOL "" FORCE) # override the default in ryml
add_subdirectory(../.. ryml)

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
