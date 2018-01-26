
add_subdirectory(${RYML_EXT_DIR}/libyaml    libyaml)
add_subdirectory(${RYML_EXT_DIR}/googletest googletest)

function(ryml_add_test test_name test_src)
  # we still have to configure ryml as a library; for now they're a
  # bunch of source files, so just include them in the executable
  # sources.
  set(t test_${test_name})
  add_executable(${t} ${test_src} ${RYML_SRC})
  add_dependencies(${t} yaml)
  target_include_directories(${t} PUBLIC "${RYML_SRC_DIR}")
  target_link_libraries(${t} yaml gtest)
endfunction()

ryml_add_test(basic basic.cpp)
