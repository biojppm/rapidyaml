cmake_minimum_required(VERSION 3.12)

project(ryml-quickstart
    DESCRIPTION "Shows how to use a custom c4core version different from ryml's c4core submodule"
    LANGUAGES CXX)
include(../../ext/c4core/cmake/c4Project.cmake)
c4_project(VERSION 0.1.0 STANDALONE
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

# download a different c4core version from the repo
c4_require_subproject(c4core REMOTE
    GIT_REPOSITORY https://github.com/biojppm/c4core.git
    GIT_TAG master)
# and now, within ryml's CMakeLists.txt, the call to
# c4_require_subproject(c4core) will use the version
# downloaded above, instead of the submodule version from
# the ryml repo:
c4_require_subproject(ryml SUBDIRECTORY ../..)

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
