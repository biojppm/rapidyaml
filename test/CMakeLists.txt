
# extern libraries

# download external libs while running cmake:
# https://crascit.com/2015/07/25/cmake-gtest/
# (via https://stackoverflow.com/questions/15175318/cmake-how-to-build-external-projects-and-include-their-targets)

set(RYML_EXT_DIR ${CMAKE_BINARY_DIR}/ryml-extern)
configure_file(extern.cmake.in ${RYML_EXT_DIR}/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${RYML_EXT_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${RYML_EXT_DIR})

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_build_samples OFF CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)
add_subdirectory(${RYML_EXT_DIR}/googletest-src ${RYML_EXT_DIR}/googletest-build)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${RYML_EXT_DIR}/libyaml-src ${RYML_EXT_DIR}/libyaml-build)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(${RYML_EXT_DIR}/yaml-cpp-src ${RYML_EXT_DIR}/yaml-cpp-build)


#include(GoogleTest) # this module requires at least cmake 3.9

# these will eventually go into their own repo
#set(c4tpl_files
#    c4/tpl/mgr.hpp c4/tpl/common.hpp c4/tpl/engine.hpp c4/tpl/rope.hpp c4/tpl/token.hpp c4/tpl/token.cpp c4/tpl/ c4/tpl/token_container.hpp c4/tpl/token_container.cpp c4/tpl/test/test_engine.cpp c4/tpl/test/test_rope.cpp
#    )

function(ryml_add_test test_name test_src)
    set(t ryml-${test_name})
    c4_add_target(RYML ${t} EXECUTABLE SANITIZE
        SOURCES test_case.hpp test_case.cpp ${test_src} ${c4tpl_files}
        LIBS ryml yaml yaml-cpp gtest
        INC_DIRS "${RYML_EXT_DIR}/googletest-src/googletest/include" "${RYML_EXT_DIR}/yaml-cpp-src/include"
        SANITIZERS t_san
        )
    if(RYML_DBG)
        target_compile_definitions(${t} PRIVATE RYML_DBG)
    endif()
    c4_add_test(ryml ${t} "${t_san}")
endfunction()

ryml_add_test(test_basic basic.cpp)
