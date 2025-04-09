option(RYML_BUILD_API_PYTHON "Enable Python API" ON)
option(RYML_API_TESTS "Enable API tests" ${RYML_BUILD_TESTS})
option(RYML_API_BENCHMARKS "Enable API tests" ${RYML_BUILD_BENCHMARKS})

c4_log("enabling API")

cmake_policy(PUSH)
# UseSWIG generates standard target names.
# https://cmake.org/cmake/help/latest/policy/CMP0078.html
cmake_policy(SET CMP0078 NEW)
# UseSWIG honors SWIG_MODULE_NAME via -module flag.
# https://cmake.org/cmake/help/latest/policy/CMP0086.html
cmake_policy(SET CMP0086 NEW)
# Modules FindPython3, FindPython2 and FindPython use LOCATION for lookup strategy.
# https://cmake.org/cmake/help/latest/policy/CMP0094.html
cmake_policy(SET CMP0094 NEW)

find_package(SWIG REQUIRED)
c4_log("found swig ${SWIG_VERSION}: ${SWIG_EXECUTABLE}")

# https://cmake.org/cmake/help/v3.13/module/UseSWIG.html
include(UseSWIG)

if(NOT RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS)
    message(FATAL_ERROR "API requires exceptions")
endif()

set(RYML_API_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RYML_SWIG_SRC ${RYML_API_DIR}/ryml.i)

include(../ext/c4core/cmake/TargetArchitecture.cmake)
c4_get_architecture_defines(RYML_SWIG_ARCH_DEFINES)
c4_log("add CPU architecture defines ${RYML_SWIG_ARCH_DEFINES}")

foreach(f ${RYML_SWIG_SRC})
    set_source_files_properties(${f} PROPERTIES
        CPLUSPLUS ON
        SWIG_MODULE_NAME ryml
        SWIG_FLAGS "-includeall"
        #INCLUDE_DIRECTORIES "${RYML_SRC_DIR}" # this needs quotes here!
        )
endforeach()


add_custom_target(ryml-api-build)
c4_set_folder_remote_project_targets("api" ryml-api-build)


if(RYML_API_TESTS)
    add_custom_target(ryml-api-test
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure ${${_c4_uprefix}CTEST_OPTIONS} -C $<CONFIG> -R "*api*"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    c4_set_folder_remote_project_targets("test" ryml-api-test)
endif()


if(RYML_API_BENCHMARKS)
    add_custom_target(ryml-api-bm)
    c4_set_folder_remote_project_targets("bm" ryml-api-bm)
endif()


if(RYML_BUILD_API_PYTHON)
    c4_log("enabling python3 API")
    set(Python3_FIND_VIRTUALENV "FIRST")
    if(APPLE)
        find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    else()
        # use Development.Module to ensure this works with cibuildwheel:
        # https://github.com/pypa/cibuildwheel/issues/639
        find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)
    endif()
    c4_log("found python:
    ver=${Python3_VERSION}
    exe=${Python3_EXECUTABLE}
    lib=${Python3_LIBRARIES}
    inc=${Python3_INCLUDE_DIRS}
")
    #
    set(t ryml-api-python3) # the target name
    set(g ${CMAKE_CURRENT_BINARY_DIR}/src/python3) # where to put c++ generated srcs
    set(r ${CMAKE_CURRENT_BINARY_DIR}/python3) # where to put the py files/libs
    #
    # alternative 1: roll out the extension using cmake
    #
    c4_get_transitive_property(ryml SOURCES ryml_srcs)
    c4_get_transitive_property(ryml INCLUDE_DIRECTORIES ryml_incs)
    swig_add_library(${t}
        LANGUAGE python
        OUTPUT_DIR ${r}
        OUTFILE_DIR ${g}
        SOURCES ${RYML_SWIG_SRC} ${ryml_srcs})
    #c4_set_folder_remote_project_targets("api" ${t})
    add_dependencies(ryml-api-build ${t})
    target_include_directories(${t} PUBLIC ${ryml_incs})
    swig_link_libraries(${t} ${Python3_LIBRARIES})
    set_target_properties(${t} PROPERTIES
        OUTPUT_NAME "ryml"
        SWIG_GENERATED_INCLUDE_DIRECTORIES ${Python3_INCLUDE_DIRS}
        ARCHIVE_OUTPUT_DIRECTORY "${r}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${r}"
        RUNTIME_OUTPUT_DIRECTORY "${r}")
    if(WIN32)
        target_compile_definitions(${t} PUBLIC __WIN32__)
    elseif(APPLE)
        set_property(TARGET ${t} APPEND PROPERTY LINK_FLAGS "-flat_namespace")
    endif()
    c4_get_transitive_property(ryml COMPILE_DEFINITIONS ryml_defs)
    target_compile_definitions(${t} PUBLIC ${RYML_SWIG_ARCH_DEFINES} ${ryml_defs})

    # Install the SWIG .so/.dll file
    install(
        TARGETS ${t}
        COMPONENT python
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

    get_property(WRAPPER_PY_FILE
        TARGET ${t}
        PROPERTY SWIG_SUPPORT_FILES)

    # Install the .py file
    install(
        FILES ${WRAPPER_PY_FILE}
        COMPONENT python
        DESTINATION ${CMAKE_INSTALL_PREFIX})

    set(pydir ${CMAKE_CURRENT_LIST_DIR}/python)
    if(RYML_API_TESTS)
        add_custom_target(ryml-api-test-python3
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure ${${_c4_uprefix}CTEST_OPTIONS} -C $<CONFIG> -R ".*python3.*"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${t}
            )
        add_dependencies(ryml-api-test ryml-api-test-python3)
        c4_set_folder_remote_project_targets("test" ryml-api-test-python3)
        function(add_python_test script)
            get_filename_component(script_name ${script} NAME_WE)
            set(script ${pydir}/tests/${script})
            set(tn ryml-api-test-python3-${script_name})
            set(cmd python ${script})
            add_custom_target(${tn}
                COMMAND ${cmd}
                DEPENDS ${t})
            c4_set_folder_remote_project_targets("test" ${tn})
            add_test(NAME ${tn}-run
                COMMAND ${cmd})
        endfunction()
        add_python_test(test_readme.py)
        add_python_test(test_parse.py)
    endif()

    if(RYML_API_BENCHMARKS)
        add_custom_target(ryml-api-bm-python3)
        add_dependencies(ryml-api-bm ryml-api-bm-python3)
        c4_set_folder_remote_project_targets("bm" ryml-api-bm-python3)
        set(script ${pydir}/ryml/tests/parse_bm.py)
        c4_add_benchmark_cmd(ryml-api-bm-python3-travis
            COMMAND python ${script} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/travis.yml ryml)
        c4_add_benchmark_cmd(ryml-api-bm-python3-appveyor
            COMMAND python ${script} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml ryml)
        c4_add_benchmark_cmd(ryml-api-bm-python3-compile_commands
            COMMAND python ${script} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/compile_commands.json ryml)
        c4_set_folder_remote_project_targets("bm" ryml-bm-api-python3-travis)
        c4_set_folder_remote_project_targets("bm" ryml-bm-api-python3-appveyor)
        c4_set_folder_remote_project_targets("bm" ryml-bm-api-python3-compile_commands)
    endif()
endif()


cmake_policy(POP)
