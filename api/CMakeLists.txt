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


set(RYML_SWIG_SRC ${CMAKE_CURRENT_LIST_DIR}/ryml.i)
set(incdirs
    ${RYML_SRC_DIR}
    ${RYML_EXT_DIR}/c4core/src
    ${RYML_EXT_DIR}/c4core/extern)
foreach(f ${RYML_SWIG_SRC})
    set_source_files_properties(${f} PROPERTIES
        CPLUSPLUS ON
        SWIG_FLAGS "-includeall"
        INCLUDE_DIRECTORIES "${incdirs}" # this needs quotes here!
        )
endforeach()


if(RYML_BUILD_API_PYTHON3)
    message(STATUS "ryml: enabling python3 API")
    find_package(Python3 COMPONENTS Interpreter Development)
    if(NOT Python3_FOUND)
        message(FATAL_ERROR "cannot find python3")
    endif()
    message(STATUS "found python ${Python3_VERSION}: ${Python3_EXECUTABLE}")
    #
    set(t ryml-api-python3)
    set(g ${CMAKE_CURRENT_BINARY_DIR}/src/python3) # where to put c++ generated srcs
    set(r ${CMAKE_CURRENT_BINARY_DIR}/python3) # where to put the py files/libs
    #
    swig_add_library(${t}
        LANGUAGE python
        OUTPUT_DIR ${r}
        OUTFILE_DIR ${g}
        SOURCES ${RYML_SWIG_SRC})
    swig_link_libraries(${t} ryml ${Python3_LIBRARIES})
    set_target_properties(${t} PROPERTIES
        OUTPUT_NAME "ryml"
        SWIG_GENERATED_INCLUDE_DIRECTORIES ${Python3_INCLUDE_DIRS}
        ARCHIVE_OUTPUT_DIRECTORY "${r}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${r}"
        RUNTIME_OUTPUT_DIRECTORY "${r}")
endif()


cmake_policy(POP)
