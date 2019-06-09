c4_setup_benchmarks(ryml)

# -----------------------------------------------------------------------------
# json libs that will be compared with ryml (and the other yaml libs)

set(_ed ${CMAKE_CURRENT_BINARY_DIR}/extern) # casual ryml extern dir (these projects are not part of ryml and are downloaded and compiled on the fly)

# jsoncpp needs to be compiled
c4_override(JSONCPP_WITH_TESTS OFF)
c4_override(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
c4_override(JSONCPP_WITH_WARNING_AS_ERROR OFF)
c4_override(JSONCPP_WITH_STRICT_ISO OFF)
c4_override(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
c4_override(JSONCPP_WITH_CMAKE_PACKAGE OFF)
ryml_require_module(jsoncpp REMOTE
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
    GIT_TAG master)
c4_set_folder_remote_project_targets(ext jsoncpp_lib)

# nlohmannjson needs to be compiled
c4_override(JSON_BuildTests OFF)
c4_override(JSON_Install OFF)
c4_override(JSON_MultipleHeaders OFF)
ryml_require_module(nlohmann_json REMOTE
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG master)

# rapidjson is header only
ryml_download_remote_proj(rapidjson ${_ed}/rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson
    GIT_TAG version1.1.0)
set(RYML_RAPIDJSON_INC_DIR ${_ed}/rapidjson/src/include)

# sajson is header only
ryml_download_remote_proj(sajson ${_ed}/sajson
    GIT_REPOSITORY https://github.com/chadaustin/sajson
    GIT_TAG master)
set(RYML_SAJSON_INC_DIR ${_ed}/sajson/src/include)


# -----------------------------------------------------------------------------
function(ryml_add_bm bm_name case_file)
    set(bm ryml-benchmark-${bm_name})
    if(NOT TARGET ${bm})
        c4_add_executable(RYML ${bm}
            SOURCES ${bm_name}.cpp ${ARGN}
            LIBS ryml yaml yaml-cpp benchmark jsoncpp_lib nlohmann_json c4fs
            INC_DIRS ${RYML_RAPIDJSON_INC_DIR} ${RYML_SAJSON_INC_DIR}
            FOLDER ryml-benchmark)
        if(RYML_DBG)
            target_compile_definitions(${bm} PRIVATE RYML_DBG)
        endif()
    endif()
    set(rdir ${CMAKE_CURRENT_BINARY_DIR}/results)
    get_filename_component(case ${case_file} NAME_WE)
    set(rt ${bm}-${case})
    set(rf ${rdir}/${bm_name}-${case}.json)
    if(NOT EXISTS ${rdir})
        file(MAKE_DIRECTORY ${rdir})
    endif()
    get_filename_component(ext ${case_file} EXT)
    if(NOT ("${ext}" STREQUAL ".json"))
        set(filter_json "--benchmark_filter='yml|yaml'")
    endif()
    c4_add_benchmark(RYML ${bm} ${rt} ${rdir} "${rt}: results are in ${rf}"
        # these are arguments to the benchmark
        ${filter_json} --benchmark_out_format=json --benchmark_out=${rf} ${case_file})
endfunction()

set(cdir "${CMAKE_CURRENT_LIST_DIR}/cases")
file(GLOB bm_cases RELATIVE "${cdir}" "${cdir}/*.*")
foreach(case_file ${bm_cases})
    ryml_add_bm(parse "${cdir}/${case_file}")
endforeach()
