add_subdirectory(${RYML_EXT_DIR}/googletest googletest)
set(BUILD_TESTING OFF)
add_subdirectory(${RYML_EXT_DIR}/libyaml    libyaml)
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory(${RYML_EXT_DIR}/yaml-cpp   yaml-cpp) # this includes googletest

include(GoogleTest) # this module requires at least cmake 3.9

function(ryml_add_test test_name test_src)
  # we still have to configure ryml as a library; for now they're a
  # bunch of source files, so just include them in the executable
  # sources.
  set(t test_${test_name})
  add_executable(${t} ${test_src} ${RYML_SRC} test_case.hpp test_case.cpp)
  add_dependencies(${t} yaml yaml-cpp)
  target_include_directories(${t} PUBLIC "${RYML_SRC_DIR}" "${GTEST_INCLUDE_DIRS}" "${RYML_EXT_DIR}/yaml-cpp/include")
  target_link_libraries(${t} yaml yaml-cpp gtest)
endfunction()

ryml_add_test(basic basic.cpp)
