
function(ryml_add_test test_name test_src)
  set(t test_${test_name})
  add_executable(${t} ${test_src})
  add_dependencies(${t} yaml)
  target_include_directories(${t} PUBLIC "${RYML_SRC_DIR}")
  target_link_libraries(${t} yaml)
endfunction()

ryml_add_test(basic basic.cpp)
