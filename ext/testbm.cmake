# these are used both for testing and benchmarking

function(ryml_testbm_link_with_c4core target)
    if(RYML_SYSTEM_C4CORE)
        target_link_libraries(${target} PUBLIC c4core::c4core)
    elseif(NOT RYML_STANDALONE)
        target_link_libraries(${target} PUBLIC c4core)
    else() # RYML_STANDALONE
        target_link_libraries(${target} PUBLIC ryml) # link with ryml to get c4core
        ryml_add_c4core_dev_to_target(${target})
        target_include_directories(${target} PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/c4core.src
            ${CMAKE_CURRENT_LIST_DIR}/c4core.dev
        )
    endif()
endfunction()


set(C4FS_DIR ${CMAKE_CURRENT_BINARY_DIR}/subprojects/c4fs)
c4_download_remote_proj(c4fs C4FS_DIR
    GIT_REPOSITORY https://github.com/biojppm/c4fs
    GIT_TAG master)
c4_add_library(c4fs LIBRARY_TYPE STATIC
    SOURCES
        ${C4FS_DIR}/src/c4/fs/fs.hpp
        ${C4FS_DIR}/src/c4/fs/fs.cpp
    INC_DIRS
        ${C4FS_DIR}/src
    FOLDER
        ext
)
ryml_testbm_link_with_c4core(c4fs)
if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(c4fs PUBLIC C4FS_SHARED
        $<BUILD_INTERFACE:C4FS_EXPORTS>
    )
endif()


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
    FOLDER
        ext
)
ryml_testbm_link_with_c4core(c4log)
if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(c4log PUBLIC C4LOG_SHARED
        $<BUILD_INTERFACE:C4LOG_EXPORTS>
    )
endif()
