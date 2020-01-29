option(RYML_BUILD_API_PYTHON3 "Enable Python3 API" ON)
option(RYML_BUILD_API_PYTHON2 "Enable Python2 API" OFF)

c4_log("enabling API")

cmake_policy(PUSH)
cmake_policy(SET CMP0078 NEW) # https://cmake.org/cmake/help/v3.14/policy/CMP0078.html
cmake_policy(SET CMP0086 NEW) # https://cmake.org/cmake/help/v3.14/policy/CMP0086.html

find_package(SWIG)
if(NOT SWIG_FOUND)
    message(FATAL_ERROR "cannot find swig")
endif()
c4_log("found swig ${SWIG_VERSION}: ${SWIG_EXECUTABLE}")
# https://cmake.org/cmake/help/v3.13/module/UseSWIG.html
include(UseSWIG)

set(RYML_API_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RYML_SWIG_SRC ${RYML_API_DIR}/ryml.i)

foreach(f ${RYML_SWIG_SRC})
    set_source_files_properties(${f} PROPERTIES
        CPLUSPLUS ON
        SWIT_MODULE_NAME ryml
        SWIG_FLAGS "-includeall"
        INCLUDE_DIRECTORIES "${incdirs}" # this needs quotes here!
        )
endforeach()


function(create_setup_py src dst)
    set(opt0arg
    )
    set(opt1arg
        NAME
        VERSION
        AUTHOR
        DESC
        SRC
    )
    set(optnarg
        PYMODS
    )
    cmake_parse_arguments("" "${opt0arg}" "${opt1arg}" "${optnarg}" "${ARGN}")
    configure_file("${src}" "${dst}" @ONLY)
endfunction()


function(pylist li out)
    print_var(li)
    set(l)
    foreach(f ${li})
        set(l "${l}'${f}', ")
    endforeach()
    set(${out} ${l} PARENT_SCOPE)
endfunction()


if(RYML_BUILD_API_PYTHON3)
    c4_log("enabling python3 API")
    find_package(Python3 COMPONENTS Interpreter Development)
    if(NOT Python3_FOUND)
        message(FATAL_ERROR "cannot find python3")
    endif()
    c4_log("found python ${Python3_VERSION}: ${Python3_EXECUTABLE}")
    #
    set(t ryml-api-python3) # the target name
    set(g ${CMAKE_CURRENT_BINARY_DIR}/src/python3) # where to put c++ generated srcs
    set(r ${CMAKE_CURRENT_BINARY_DIR}/python3) # where to put the py files/libs
    #
    # alternative 1: roll out the extension using cmake
    #
    swig_add_library(${t}
        LANGUAGE python
        OUTPUT_DIR ${r}
        OUTFILE_DIR ${g}
        SOURCES ${RYML_SWIG_SRC})
    swig_link_libraries(${t} ryml-interface ${Python3_LIBRARIES})
    set_target_properties(${t} PROPERTIES
        OUTPUT_NAME "ryml"
        SWIG_GENERATED_INCLUDE_DIRECTORIES ${Python3_INCLUDE_DIRS}
        ARCHIVE_OUTPUT_DIRECTORY "${r}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${r}"
        RUNTIME_OUTPUT_DIRECTORY "${r}")
    #
    # alternative 2: use distutils to create the extension
    #
    # get the list of generated files and format it as a python string list
    #c4_get_transitive_property(ryml SOURCES ryml_srcs)
    #c4_get_transitive_property(ryml INCLUDE_DIRS ryml_incs)
    #pylist("${ryml_srcs};rymlPYTHON_wrap.cxx" ryml_srcs)
    #pylist("${ryml_incs}" ryml_incs)
    #create_setup_py(${RYML_API_DIR}/python/setup.py ${g}/setup.py
    #    NAME ryml
    #    VERSION ${RYML_VERSION}
    #    AUTHOR "${RYML_AUTHOR}"
    #    DESC "${RYML_DESC}"
    #    SRC "${genfiles_str}"
    #    )
    #add_custom_target(${t}-distutils
    #    COMMAND ${Python3_EXECUTABLE} setup.py build_ext --inplace
    #    WORKING_DIRECTORY ${g})

    function(add_python_test script)
        set(pydir ${CMAKE_CURRENT_LIST_DIR}/python)
        get_filename_component(script_name ${script} NAME_WE)
        set(script ${pydir}/${script})
        set(tn ${t}-test-${script_name})
        set(cmd env PYTHONPATH=${r} ${Python3_EXECUTABLE} ${script})
        add_custom_target(${tn}
            COMMAND ${cmd}
            DEPENDS ${t}
            WORKING_DIRECTORY ${pydir})
        if(RYML_BUILD_TESTS)
            add_test(NAME ${tn}-run
                COMMAND ${cmd}
                WORKING_DIRECTORY ${pydir})
        endif()
    endfunction()

    add_python_test(parse.py)
    #if(RYML_BUILD_BENCHMARKS)
    #    c4_add_benchmark_cmd(ryml ryml-python3-api-bm-travis-ryml
    #        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/travis.yml ryml)
    #    c4_add_benchmark_cmd(ryml ryml-python3-api-bm-appveyor-ryml
    #        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/appveyor.yml ryml)
    #    c4_add_benchmark_cmd(ryml ryml-python3-api-bm-compile_commands-ryml
    #        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../bm/cases/compile_commands.json ryml)
    #endif()
endif()


cmake_policy(POP)
