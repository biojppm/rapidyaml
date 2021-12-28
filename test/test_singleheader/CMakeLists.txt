cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ryml
    DESCRIPTION "Single header version of ryml library for testing"
    HOMEPAGE_URL "https://github.com/biojppm/rapidyaml"
    LANGUAGES CXX)
include(../../ext/c4core/cmake/c4Project.cmake)
c4_project(VERSION 0.1.7
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

set(rymldir "${CMAKE_CURRENT_LIST_DIR}/../..")
set(singleheaderdir "${rymldir}/src_singleheader")
set(singleheader "${singleheaderdir}/ryml_all.hpp")
if(NOT (EXISTS "${singleheader}"))
    # try generating a header
    set(cmd python "${rymldir}/tools/amalgamate.py" "${singleheader}")
    message(STATUS "single header not found, attempting to generate... ${cmd}")
    execute_process(COMMAND ${cmd}
        #COMMMAND_ECHO STDOUT
        RESULT_VARIABLE status)
    if((NOT (status EQUAL "0")) OR NOT (EXISTS "${singleheader}"))
        message(FATAL_ERROR "cannot find or generate single header for ryml: ${singleheader}")
    endif()
    message(STATUS "single header successfully generated: ${singleheader}")
endif()

c4_add_library(ryml
    INC_DIRS
        $<BUILD_INTERFACE:${singleheaderdir}> $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${rymldir}/src> # because of a couple of detail:: headers which are not amalgamated
    SOURCE_ROOT ${singleheaderdir}
    SOURCES
        ${singleheader}
        ${CMAKE_CURRENT_LIST_DIR}/libryml_singleheader.cpp)
target_compile_definitions(ryml PUBLIC -DRYML_SINGLE_HEADER) # this is needed only for the tests, not ryml itself

include(../../ext/testbm.cmake)

enable_testing()
set(RYML_DEFINED_FROM_SINGLEHEADER ON)
add_subdirectory(../../test test)
