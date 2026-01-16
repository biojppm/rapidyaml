# these are used both for testing and benchmarking


set(C4FS_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4fs)
c4_download_remote_proj(c4fs C4FS_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4fs
    GIT_TAG master)
c4_add_library(c4fs
    SOURCES
        ${C4FS_DIR}/src/c4/fs/fs.hpp
        ${C4FS_DIR}/src/c4/fs/fs.cpp
    INC_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/c4core.src
        ${C4FS_DIR}/src
    LIBS
        ryml
)


set(C4LOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4log)
c4_download_remote_proj(c4log C4LOG_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4log
    GIT_TAG master)
c4_add_library(c4log
    SOURCES
        ${C4LOG_DIR}/src/c4/log/log.hpp
        ${C4LOG_DIR}/src/c4/log/log.cpp
    INC_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/c4core.src
        ${C4LOG_DIR}/src
    LIBS
        ryml
)


if(RYML_STANDALONE)
    message(STATUS "wtf0: ${CMAKE_CURRENT_LIST_DIR}")
    message(STATUS "wtf1: ${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "wtf2: ${CMAKE_SOURCE_DIR}")
    add_library(ryml-c4core_dev)
    file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/c4core.dev.txt _src)
    _c4_transform_to_full_path(_src _ ${CMAKE_CURRENT_LIST_DIR}/c4core.dev)
    target_sources(ryml-c4core_dev PRIVATE ${_src})
    target_include_directories(ryml-c4core_dev PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/c4core.dev>
        ${CMAKE_CURRENT_LIST_DIR}/c4core.src)
    target_link_libraries(c4fs PRIVATE ryml-c4core_dev)
    target_link_libraries(c4log PRIVATE ryml-c4core_dev)
endif()
