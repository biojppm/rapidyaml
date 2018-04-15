c4_setup_testing(ryml)

# a test suite for YAML
c4_download_remote_proj(ryml yaml-test-suite ${CMAKE_CURRENT_BINARY_DIR}/extern/yaml-test-suite
    GIT_REPOSITORY https://github.com/yaml/yaml-test-suite
    GIT_TAG master
    )

# extern libraries

set(ryml_ext_dir ${CMAKE_CURRENT_BINARY_DIR}/extern)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
c4_import_remote_proj(ryml libyaml ${ryml_ext_dir}/libyaml
    GIT_REPOSITORY https://github.com/yaml/libyaml
    GIT_TAG master
    )

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
c4_import_remote_proj(ryml yaml-cpp ${ryml_ext_dir}/yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG master
    )

# these will eventually go into their own repo
#set(c4tpl_files
#    c4/tpl/mgr.hpp c4/tpl/common.hpp c4/tpl/engine.hpp c4/tpl/rope.hpp c4/tpl/token.hpp c4/tpl/token.cpp c4/tpl/ c4/tpl/token_container.hpp c4/tpl/token_container.cpp c4/tpl/test/test_engine.cpp c4/tpl/test/test_rope.cpp
#    )

function(ryml_add_test test_name test_src)
    set(t ryml-test-${test_name})
    c4_add_executable(RYML ${t}
        SOURCES test_case.hpp test_case.cpp ${test_src} ${c4tpl_files}
        INC_DIRS ${CMAKE_CURRENT_LIST_DIR} ${ryml_ext_dir}/yaml-cpp/src/include
        LIBS ryml yaml yaml-cpp gtest
        FOLDER ryml-test)
    if(RYML_DBG)
        target_compile_definitions(${t} PRIVATE RYML_DBG)
    endif()
    c4_add_test(RYML ${t} ON)
endfunction()

ryml_add_test(test_basic basic.cpp)
