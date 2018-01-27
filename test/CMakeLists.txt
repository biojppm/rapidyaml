add_subdirectory(${RYML_EXT_DIR}/googletest googletest)
add_subdirectory(${RYML_EXT_DIR}/libyaml    libyaml)

include(GoogleTest) # this module requires at least cmake 3.9

function(ryml_add_test test_name test_src)
  # we still have to configure ryml as a library; for now they're a
  # bunch of source files, so just include them in the executable
  # sources.
  set(t test_${test_name})
  add_executable(${t} ${test_src} ${RYML_SRC})
  add_dependencies(${t} yaml)
  target_include_directories(${t} PUBLIC "${RYML_SRC_DIR}" "${GTEST_INCLUDE_DIRS}")
  target_link_libraries(${t} yaml gtest)
endfunction()

ryml_add_test(basic basic.cpp)
