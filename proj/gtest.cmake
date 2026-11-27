function(_ryml_runcmd)
    #message(STATUS "Running command: ${ARGN}")
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE retval)
    if(NOT (${retval} EQUAL 0))
        message(FATAL_ERROR "Command failed with exit status ${retval}: ${ARGN}")
    endif()
endfunction()

function(ryml_import_gtest)
    _c4_handle_args(_ARGS ${ARGN}
        _ARGS0
        _ARGS1
          BUILD_TYPE # override the build type (do not inherit)
        _ARGSN
    )
    # choose version
    set(gtestversion 1.17.0)
    set(gtesttag v${gtestversion})
    # required: fall back when using c++11
    if(CMAKE_CXX_STANDARD VERSION_LESS_EQUAL 11)
        set(gtestversion 1.12.1)
        set(gtesttag release-${gtestversion})
    # required: fall back when using c++14
    elseif(CMAKE_CXX_STANDARD VERSION_LESS_EQUAL 14)
        # note: gtest claims v1.16.0 supports c++14, but
        # that's not the case. The latest version that we have
        # seen working with c++14 is this one:
        set(gtestversion 1.14.0)
        set(gtesttag v${gtestversion})
    endif()
    # download the project
    set(gtest_src_dir_ ${CMAKE_CURRENT_BINARY_DIR}/ext/gtest) # candidate, changed on the next call
    c4_download_remote_proj(gtest-${gtestversion} gtest_src_dir_
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG ${gtesttag} GIT_SHALLOW ON)
    if(NOT EXISTS "${gtest_src_dir_}/CMakeLists.txt")
        message(FATAL_ERROR "not found: ${gtest_src_dir_}/CMakeLists.txt")
    endif()
    # apply the patch
    if(gtest_patch_)
        _c4_get_subproject_property(gtest-${gtestversion} SRC_DIR srcdir)
        apply_patch("${gtest_patch_}" "${srcdir}" "${srcdir}/.gtest_gcc-4.8.patch")
    endif()
    # compile options:
    set(gtest_priv_flags_)
    set(gtest_publ_flags_)
    set(gtest_patch_)
    if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0) # gcc-4.x workarounds
            set(gtest_patch_ "${_c4_project_dir}/compat/gtest_gcc-4.8.patch")
            set(gtest_publ_flags_ "${gtest_publ_flags_} -std=c++11 -include ${_c4_project_dir}/compat/c4/gcc-4.8.hpp")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13)
            set(gtest_priv_flags_ "${gtest_priv_flags_} -Wno-reserved-identifier")
        endif()
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 21)
            set(gtest_priv_flags_ "${gtest_priv_flags_} -Wno-character-conversion")
        endif()
    endif()
    if(MSVC OR (NOT _BUILD_TYPE))
        c4_add_subproj(gtest-${gtestversion}
            "${gtest_src_dir_}"
            "${CMAKE_CURRENT_BINARY_DIR}/ext/gtest.build"
            OVERRIDE
                BUILD_GTEST ON
                BUILD_GMOCK OFF
                gtest_force_shared_crt ON
                gtest_build_samples OFF
                gtest_build_tests OFF
            EXCLUDE_FROM_ALL)
        c4_set_folder_remote_project_targets(ext gtest gtest_main)
        target_compile_options(gtest PRIVATE ${gtest_priv_flags_})
        target_compile_options(gtest PUBLIC ${gtest_publ_flags_})
    else()
        set(gtest_overrides_
            -DBUILD_GTEST=ON
            -DBUILD_GMOCK=OFF
            -Dgtest_force_shared_crt=ON
            -Dgtest_build_samples=OFF
            -Dgtest_build_tests=OFF
        )
        # compile using external project to avoid coverage issues
        # (gcov has trouble with the gtest files)
        set(gtest_mark_ ${CMAKE_CURRENT_BINARY_DIR}/ext/gtest.done)
        set(gtest_build_ ${CMAKE_CURRENT_BINARY_DIR}/ext/gtest.build)
        set(gtest_inst_ ${CMAKE_CURRENT_BINARY_DIR}/ext/gtest.install)
        if(EXISTS ${gtest_mark_})
            c4_log("gtest already installed in ${_BUILD_TYPE}: ${gtest_inst_}")
        else()
            c4_log("compiling gtest in ${_BUILD_TYPE}: ${gtest_inst_}")
            string(TOUPPER "${_BUILD_TYPE}" UBUILD_TYPE)
            _ryml_runcmd(${CMAKE_COMMAND} -S ${gtest_src_dir_}
                -B ${gtest_build_}
                ${gtest_overrides_}
                "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
                "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                "-DCMAKE_GENERATOR=${CMAKE_GENERATOR}"
                "-DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                "-DCMAKE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                "-DCMAKE_INSTALL_PREFIX=${gtest_inst_}"
                "-DCMAKE_BUILD_TYPE=${_BUILD_TYPE}"
                "-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
                "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
                "-DCMAKE_CXX_FLAGS=${gtest_priv_flags_} ${gtest_publ_flags_}"
                "-DCMAKE_LINKER_FLAGS=${CMAKE_LINKER_FLAGS}"
                "-DCMAKE_LINKER_FLAGS_${UBUILD_TYPE}=${CMAKE_LINKER_FLAGS_${UBUILD_TYPE}}"
            )
            _ryml_runcmd(${CMAKE_COMMAND} --build ${gtest_build_} --config ${_BUILD_TYPE} --parallel)
            _ryml_runcmd(${CMAKE_COMMAND} --install ${gtest_build_} --config ${_BUILD_TYPE})
            _ryml_runcmd(${CMAKE_COMMAND} -E touch ${gtest_mark_})
        endif(EXISTS ${gtest_mark_})
        # now have cmake find the exact installed gtest
        find_package(GTest REQUIRED CONFIG NO_MODULE
            PATHS "${gtest_inst_}"
            PATH_SUFFIXES lib lib/cmake lib/cmake/GTest
            CMAKE_FIND_ROOT_PATH_BOTH # important for cross-compiling; otherwise only the toolchain is searched
            NO_DEFAULT_PATH NO_CMAKE_SYSTEM_PATH)
        # and finally, define the imported target
        add_library(gtest INTERFACE)
        set_target_properties(gtest PROPERTIES
            INTERFACE_LINK_LIBRARIES GTest::gtest
            INTERFACE_COMPILE_OPTIONS "${gtest_public_flags_}"
            INTERFACE_COMPILE_FEATURES cxx_std_11
        )
    endif()
endfunction()
