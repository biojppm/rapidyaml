option(RYML_BUILD_API_PYTHON3 "Enable Python3 API" ON)
option(RYML_BUILD_API_PYTHON2 "Enable Python2 API" OFF)

message(STATUS "ryml: enabling API")

cmake_policy(PUSH)
cmake_policy(SET CMP0078 NEW) # https://cmake.org/cmake/help/v3.13/policy/CMP0078.html#policy:CMP0078

find_package(SWIG)
if(NOT SWIG_FOUND)
    message(FATAL_ERROR "cannot find swig")
endif()
message(STATUS "found swig ${SWIG_VERSION}: ${SWIG_EXECUTABLE}")

# https://cmake.org/cmake/help/v3.13/module/UseSWIG.html
include(UseSWIG)

set(RYML_API_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RYML_SWIG_SRC ${RYML_API_DIR}/ryml.i)

foreach(f ${RYML_SWIG_SRC})
    set_source_files_properties(${f} PROPERTIES
        CPLUSPLUS ON
        SWIG_FLAGS "-includeall"
        INCLUDE_DIRECTORIES "${incdirs}" # this needs quotes here!
        )
endforeach()


function(create_setup_py src dst)
    set(options0arg
    )
    set(options1arg
        NAME
        VERSION
        AUTHOR
        DESC
        SRC
    )
    set(optionsnarg
        PYMODS
    )
    cmake_parse_arguments("" "${options0arg}" "${options1arg}" "${optionsnarg}" "${ARGN}")
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
    message(STATUS "ryml: enabling python3 API")
    find_package(Python3 COMPONENTS Interpreter Development)
    if(NOT Python3_FOUND)
        message(FATAL_ERROR "cannot find python3")
    endif()
    message(STATUS "found python ${Python3_VERSION}: ${Python3_EXECUTABLE}")
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
endif()


cmake_policy(POP)
