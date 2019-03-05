c4_setup_benchmarks(ryml)

c4_import_remote_proj(ryml c4fs ${ryml_ext_dir}/c4fs
    GIT_REPOSITORY https://github.com/biojppm/c4fs
    GIT_TAG master)

function(ryml_add_bm bm_name case_file)
    set(bm ryml-benchmark-${bm_name})
    if(NOT TARGET ${bm})
        c4_add_executable(RYML ${bm}
            SOURCES ${bm_name}.cpp ${ARGN}
            LIBS ryml yaml yaml-cpp benchmark c4fs
            FOLDER ryml-benchmark)
        if(RYML_DBG)
            target_compile_definitions(${bm} PRIVATE RYML_DBG)
        endif()
    endif()
    set(rdir ${CMAKE_CURRENT_BINARY_DIR}/bm_results)
    get_filename_component(case ${case_file} NAME_WE)
    set(rt ${bm}-${case})
    set(rf ${rdir}/${bm_name}-${case}.json)
    print_var(bm)
    print_var(rt)
    print_var(rf)
    if(NOT EXISTS ${rdir})
        file(MAKE_DIRECTORY ${rdir})
    endif()
    c4_add_benchmark(RYML ${bm} ${rt} ${rdir} "${rt}: results are in ${rf}"
        --benchmark_out_format=json --benchmark_out=${rf} ${case_file})
endfunction()

set(cdir "${CMAKE_CURRENT_LIST_DIR}/cases")
file(GLOB bm_cases RELATIVE "${cdir}" "${cdir}/*.*")
print_var(bm_cases)
foreach(case_file ${bm_cases})
    print_var(case_file)
    ryml_add_bm(parse "${cdir}/${case_file}")
endforeach()
