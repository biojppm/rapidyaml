
set(C4FS_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4fs)
c4_download_remote_proj(c4fs C4FS_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4fs
    GIT_TAG master)
c4_add_library(c4fs
    SOURCES
        ${C4FS_DIR}/src/c4/fs/fs.hpp
        ${C4FS_DIR}/src/c4/fs/fs.cpp
    INC_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/c4core/src
        ${C4FS_DIR}/src
    LIBS
        ryml
    FOLDER
        ext
)
