# these are used both for testing and benchmarking
set(C4FS_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4fs)
c4_download_remote_proj(c4fs C4FS_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4fs
    GIT_TAG master)
add_library(c4fs)
target_sources(c4fs
    PUBLIC ${C4FS_DIR}/src/c4/fs/fs.hpp
    PRIVATE ${C4FS_DIR}/src/c4/fs/fs.cpp)
target_include_directories(c4fs PUBLIC
    ${C4FS_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/c4core.src
    ${CMAKE_CURRENT_LIST_DIR}/c4core.dev
)

set(C4LOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4log)
c4_download_remote_proj(c4log C4LOG_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4log
    GIT_TAG master)
add_library(c4log)
target_sources(c4log
    PUBLIC
        ${C4LOG_DIR}/src/c4/log/log.hpp
    PRIVATE
        ${C4LOG_DIR}/src/c4/log/log.cpp
        ${CMAKE_CURRENT_LIST_DIR}/c4core.dev/c4/memory_resource.hpp
        ${CMAKE_CURRENT_LIST_DIR}/c4core.dev/c4/memory_resource.cpp)
target_include_directories(c4log PUBLIC
    ${C4LOG_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/c4core.src
    ${CMAKE_CURRENT_LIST_DIR}/c4core.dev
)
