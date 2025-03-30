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
        ${C4FS_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/c4core.src
        ${CMAKE_CURRENT_LIST_DIR}/c4core.dev
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
        ${C4LOG_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/c4core.src
        ${CMAKE_CURRENT_LIST_DIR}/c4core.dev
    LIBS
        ryml
)
