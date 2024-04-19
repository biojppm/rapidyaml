#!/usr/bin/env bash

set -e
set -x

PROJ_DIR=$(pwd)

function c4_show_info()
{
    set +x
    env | sort
    echo "PROJ_DIR=$PROJ_DIR"
    echo "PROJ_PFX_TARGET=$PROJ_PFX_TARGET"
    echo "PROJ_PFX_CMAKE=$PROJ_PFX_CMAKE"
    echo "CMAKE_FLAGS=$CMAKE_FLAGS"
    echo "NUM_JOBS_BUILD=$NUM_JOBS_BUILD"
    echo "GITHUB_WORKSPACE=$GITHUB_WORKSPACE"
    pwd
    ls -lFhp
    echo "BITLINKS=$BITLINKS"
    for bl in shared64 static64 shared32 static32 arm64 arm64shared arm64static shared64arm static64arm arm32 arm32shared arm32static shared32arm static32arm arm ; do
        if _c4skipbitlink $bl ; then
            echo "skip $bl"
        else
            echo "exec $bl"
        fi
    done
    echo "CXX_=$CXX_"
    echo "BT=$BT"
    echo "LINT=$LINT"
    echo "SAN=$SAN"
    echo "SAN_ONLY=$SAN"
    echo "VG=$VG"
    echo "BM=$BM"
    echo "STD=$STD"
    echo "ARM=$ARM"
    echo "LIBCXX=$LIBCXX"
    echo "VERBOSE_MAKEFILES=$VERBOSE_MAKEFILES"
    which cmake
    cmake --version
    case "$CXX_" in
        xcode)
            # https://gist.github.com/nlutsenko/ee245fbd239087d22137
            echo "number of cores=$(sysctl -n hw.ncpu)"
            #defaults read com.apple.dt.xcodebuild | grep -i Number | grep -i Build
            #defaults read com.apple.dt.Xcode | grep -i Number | grep -i Tasks
            ;;
        gcc*|g++*|*clang*)
            echo "number of cores=$(nproc)"
            $CXX_ --version
            ;;
        *)
            ;;
    esac
    set -x
    git branch
    git rev-parse HEAD
    git tag || echo
    git log -1 --format='%H'
}

function _c4bits()
{
    case "$1" in
        shared64|static64|static64arm|shared64arm|arm64static|arm64shared|arm64) echo 64 ;;
        shared32|static32|static32arm|shared32arm|arm32static|arm32shared|arm32|arm) echo 32 ;;
        *) exit 1 ;;
    esac
}

function _c4linktype()
{
    case "$1" in
        shared64|shared32|arm64static|arm32static|static64arm|static32arm) echo shared ;;
        static64|static32|arm64shared|arm32shared|shared64arm|shared32arm|arm64|arm32|arm) echo static ;;
        *) exit 1 ;;
    esac
}

function _c4vsarchtype()
{
    # https://cmake.org/cmake/help/git-stage/generator/Visual%20Studio%2016%202019.html
    case "$1" in
        shared64|static64) echo x64 ;;
        shared32|static32) echo Win32 ;;
        arm64|arm64shared|arm64static|shared64arm|static64arm) echo ARM64 ;;
        arm32|arm32shared|arm32static|shared32arm|static32arm|arm) echo ARM ;;
        *) exit 1 ;;
    esac
}

function _c4skipbitlink()
{
    bitlink___=$1
    if [ -z "$BITLINKS" ] ; then
        return 1  # return nonzero as failure, meaning DO NOT SKIP
    fi
    for bl___ in $BITLINKS ; do
        if [ "${bl___}" == "${bitlink___}" ] ; then
            return 1  # return nonzero as failure, meaning DO NOT SKIP
        fi
    done
    return 0  # return nonzero as success, meaning DO SKIP
}

function c4_build_test()
{
    c4_build_target $* test-build
}

function c4_run_test()
{
    c4_run_target $* test
}

function c4_build_target()  # runs in parallel
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    target=$2
    if [ ! -z "$target" ] ; then
        target="--target $target"
    fi
    build_dir=`pwd`/build/$id
    export CTEST_OUTPUT_ON_FAILURE=1
    # watchout: the `--parallel` flag to `cmake --build` is broken:
    # https://discourse.cmake.org/t/parallel-does-not-really-enable-parallel-compiles-with-msbuild/964/10
    # https://gitlab.kitware.com/cmake/cmake/-/issues/20564
    cmake --build $build_dir --config $BT $target --verbose -- $(_c4_generator_build_flags) $(_c4_parallel_build_flags)
}

function c4_run_target()  # does not run in parallel
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    target=$2
    build_dir=`pwd`/build/$id
    export CTEST_OUTPUT_ON_FAILURE=1
    cmake --build $build_dir --config $BT --target $target -- $(_c4_generator_build_flags)
}

function c4_package()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    generator=$2
    build_dir=`pwd`/build/$id
    if [ -z "$generator" ] ; then
        c4_run_target $id package
    else
        ( cd $build_dir ; cpack -G $generator )
    fi
}

function c4_submit_coverage()
{
    if [ "$BT" != "Coverage" ] ; then
        echo "build type is \"$BT\": no coverage to submit"
        return 0
    fi
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    coverage_service=$2
    build_dir=`pwd`/build/$id
    echo "Submitting coverage data: $build_dir --> $coverage_service"
    cmake --build $build_dir --config $BT --target ${PROJ_PFX_TARGET}coverage-submit-$coverage_service
}

# WIP
function c4_run_static_analysis()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    linktype=$(_c4linktype $id)
    build_dir=`pwd`/build/$id
    # https://blog.kitware.com/static-checks-with-cmake-cdash-iwyu-clang-tidy-lwyu-cpplint-and-cppcheck/
    pushd $PROJ_DIR
}

function c4_cfg_test()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    id=$1
    #
    build_dir=`pwd`/build/$id
    install_dir=`pwd`/install/$id
    mkdir -p $build_dir
    mkdir -p $install_dir
    #
    if [ "$TOOLCHAIN" != "" ] ; then
        toolchain_file=`pwd`/$TOOLCHAIN
        if [ ! -f "$toolchain_file" ] ; then
            echo "ERROR: toolchain not found: $toolchain_file"
            exit 1
        fi
        _addcmkflags -DCMAKE_TOOLCHAIN_FILE=$toolchain_file
    else
        bits=$(_c4bits $id)
        linktype=$(_c4linktype $id)
        case "$linktype" in
            static) _addcmkflags -DBUILD_SHARED_LIBS=OFF ;;
            shared) _addcmkflags -DBUILD_SHARED_LIBS=ON ;;
            *)
                echo "ERROR: unknown linktype: $linktype"
                exit 1
                ;;
        esac
    fi
    if [ "$STD" != "" ] ; then
        _addcmkflags -DC4_CXX_STANDARD=$STD
        _addprojflags CXX_STANDARD=$STD
    fi
    if [ "$LIBCXX" != "" ] ; then
        _addprojflags USE_LIBCXX=$LIBCXX
    fi
    #
    if [ "$DEV" != "OFF" ] ; then
        _addprojflags DEV=ON
    fi
    case "$LINT" in
        all       ) _addprojflags LINT=ON LINT_TESTS=ON LINT_CLANG_TIDY=ON  LINT_PVS_STUDIO=ON ;;
        clang-tidy) _addprojflags LINT=ON LINT_TESTS=ON LINT_CLANG_TIDY=ON  LINT_PVS_STUDIO=OFF ;;
        pvs-studio) _addprojflags LINT=ON LINT_TESTS=ON LINT_CLANG_TIDY=OFF LINT_PVS_STUDIO=ON ;;
        *         ) _addprojflags LINT=OFF ;;
    esac
    case "$SAN" in
        ALL) _addprojflags SANITIZE=ON ;;
        A  ) _addprojflags SANITIZE=ON ASAN=ON  TSAN=OFF MSAN=OFF UBSAN=OFF ;;
        T  ) _addprojflags SANITIZE=ON ASAN=OFF TSAN=ON  MSAN=OFF UBSAN=OFF ;;
        M  ) _addprojflags SANITIZE=ON ASAN=OFF TSAN=OFF MSAN=ON  UBSAN=OFF ;;
        UB ) _addprojflags SANITIZE=ON ASAN=OFF TSAN=OFF MSAN=OFF UBSAN=ON ;;
        *  ) _addprojflags SANITIZE=OFF ;;
    esac
    case "$SAN_ONLY" in
        ON) _addprojflags SANITIZE_ONLY=ON ;;
        * ) _addprojflags SANITIZE_ONLY=OFF ;;
    esac
    case "$VG" in
        ON) _addprojflags VALGRIND=ON VALGRIND_SGCHECK=OFF ;; # FIXME SGCHECK should be ON
        * ) _addprojflags VALGRIND=OFF VALGRIND_SGCHECK=OFF ;;
    esac
    case "$BM" in
        ON) _addprojflags BUILD_BENCHMARKS=ON ;;
        * ) _addprojflags BUILD_BENCHMARKS=OFF ;;
    esac
    if [ "$BT" == "Coverage" ] ; then
        # the coverage repo tokens can be set in the travis environment:
        # export CODECOV_TOKEN=.......
        # export COVERALLS_REPO_TOKEN=.......
        _addprojflags COVERAGE_CODECOV=ON COVERAGE_CODECOV_SILENT=OFF
        _addprojflags COVERAGE_COVERALLS=ON COVERAGE_COVERALLS_SILENT=OFF
    fi
    if [ ! -z "$VERBOSE_MAKEFILES" ] ; then
        _addcmkflags -DCMAKE_VERBOSE_MAKEFILES=$VERBOSE_MAKEFILES
    fi
    _addcmkflags -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    if [ ! -z "$CMAKE_FLAGS" ] ; then
        _addcmkflags $CMAKE_FLAGS
    fi
    # do this before setting CMAKE_C_FLAGS
    case "$CXX_" in
        vs*)
            # WATCHOUT: leave a leading space in the _FLAGS options!
            # This is needed because bash will expand a leading
            # /DWIN32 to the fs root, ie to something like
            # C:/Git/DWIN32. The leading space prevents this.
            #
            # see https://github.com/bmatzelle/gow/issues/196
            CFLAGS=" /DWIN32 /D_WINDOWS $CFLAGS"
            CXXFLAGS=" /DWIN32 /D_WINDOWS /EHsc /GR $CXXFLAGS"
            ;;
        xcode) ;;
        arm*|"") # make sure arm* comes before *g++ or *gcc*
            ;;
        *g++*|*gcc*|*clang*)
            CFLAGS="-m$bits $CFLAGS"
            CXXFLAGS="-m$bits $CXXFLAGS"
            ;;
        em++)
            CFLAGS="-s DISABLE_EXCEPTION_CATCHING=0 $CFLAGS"
            CXXFLAGS="-s DISABLE_EXCEPTION_CATCHING=0 $CXXFLAGS"
            ;;
        *)
            echo "unknown compiler"
            exit 1
            ;;
    esac

    echo "building with additional cmake flags: $CMFLAGS"

    export C4_EXTERN_DIR=`pwd`/build/extern
    mkdir -p $C4_EXTERN_DIR

    cmake --version
    pwd

    #
    # bash quote handling is a fiasco, and I could not find a way of storing
    # quoted strings in variables and then expand the variables with correct quotes
    # so we have to do this precious jewell of chicanery:
    case "$CXX_" in
        vs2022)
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -G 'Visual Studio 17 2022' -A $(_c4vsarchtype $id) \
                  $(_c4_add_ehsc_to_vs_arm32 $id) \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_FLAGS=" $CFLAGS" -DCMAKE_CXX_FLAGS=" $CXXFLAGS"
            ;;
        vs2019)
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -G 'Visual Studio 16 2019' -A $(_c4vsarchtype $id) \
                  $(_c4_add_ehsc_to_vs_arm32 $id) \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_FLAGS=" $CFLAGS" -DCMAKE_CXX_FLAGS=" $CXXFLAGS"
            ;;
        vs2017)
            case "$bits" in
                64) g="Visual Studio 15 2017 Win64" ;;
                32) g="Visual Studio 15 2017" ;;
                *) exit 1 ;;
            esac
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  $(_c4_add_ehsc_to_vs_arm32 $id) \
                  -DCMAKE_BUILD_TYPE=$BT -G "$g" $CMFLAGS \
                  -DCMAKE_C_FLAGS=" $CFLAGS" -DCMAKE_CXX_FLAGS=" $CXXFLAGS"
            ;;
        xcode)
            g=Xcode
            case "$bits" in
                64) a="x86_64" ;;
                32) a="i386"
                    echo "xcode does not support i386"
                    exit 1 # i386 is deprecated in xcode
                    ;;
            esac
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT -G "$g" \
                  -DCMAKE_OSX_ARCHITECTURES=$a $CMFLAGS \
                  -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS"
            ;;
        *mingw*)
            export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -G "MinGW Makefiles" $CMFLAGS \
                  -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS"
            ;;
        arm*|"") # make sure arm* comes before *g++ or *gcc*
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS"
            ;;
        *g++*|*gcc*|*clang*)
            export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')
            _c4_choose_clang_tidy $CXX_
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_C_COMPILER=$CC_ -DCMAKE_CXX_COMPILER=$CXX_ \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS"
            cmake --build $build_dir --target help | sed 1d | sort
            ;;
        em++)
            emcmake cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS"
            ;;
        *)
            echo "unknown compiler"
            exit 1
            ;;
    esac
}

function _c4_choose_clang_tidy()
{
    cxx=$1
    # only for clang compilers.
    case $cxx in
        clang*)
            # try with version first
            clang_tidy_ver=$(echo $cxx | sed "s:++:-tidy:")
            clang_tidy=$(echo $cxx | sed "s:++.*:-tidy:")
            for n in $clang_tidy_ver $clang_tidy ; do
                exe=$(which $n)
                echo "searching for $n: $exe"
                if [ -z "$exe" ] ; then
                    echo "could not find $clang_tidy"
                else
                    _addcmkflags "-DCLANG_TIDY=$exe"
                    return 0
                fi
            done
            echo "error: could not find clang-tidy for $cxx"
            exit 1
            ;;
    esac
}

# add cmake flags without project prefix
function _addcmkflags()
{
    for f in $* ; do
        CMFLAGS="$CMFLAGS ${f}"
    done
}

# add cmake flags with project prefix
function _addprojflags()
{
    for f in $* ; do
        CMFLAGS="$CMFLAGS -D${PROJ_PFX_CMAKE}${f}"
    done
}

function _c4_add_ehsc_to_vs_arm32()
{
    id=$1
    case "$CXX_" in
        vs*)
            case "$id" in
                arm32|arm32shared|arm32static|shared32arm|static32arm|arm)
                    echo '-DCMAKE_CXX_FLAGS="/EHsc"'
                    ;;
                *)
            esac
            ;;
    esac
}

function _c4_parallel_build_flags()
{
    case "$CXX_" in
        vs2022|vs2019|vs2017|vs2015)
            # https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild-command-line-reference?view=vs-2019
            # https://stackoverflow.com/questions/2619198/how-to-get-number-of-cores-in-win32
            if [ -z "$NUM_JOBS_BUILD" ] ; then
                echo "/maxcpucount:$NUMBER_OF_PROCESSORS"
            else
                echo "/maxcpucount:$NUM_JOBS_BUILD"
            fi
            ;;
        xcode)
            # https://stackoverflow.com/questions/5417835/how-to-modify-the-number-of-parallel-compilation-with-xcode
            # https://gist.github.com/nlutsenko/ee245fbd239087d22137
            if [ -z "$NUM_JOBS_BUILD" ] ; then
                echo "-IDEBuildOperationMaxNumberOfConcurrentCompileTasks=$(sysctl -n hw.ncpu)"
            else
                echo "-IDEBuildOperationMaxNumberOfConcurrentCompileTasks=$NUM_JOBS_BUILD"
            fi
            ;;
        *g++*|*gcc*|*clang*|em++)
            if [ -z "$NUM_JOBS_BUILD" ] ; then
                echo "-j $(nproc)"
            else
                echo "-j $NUM_JOBS_BUILD"
            fi
            ;;
        "") # allow empty compiler
            ;;
        *)
            echo "unknown compiler"
            exit 1
            ;;
    esac
}

function _c4_generator_build_flags()
{
    case "$CXX_" in
        vs2022|vs2019|vs2017|vs2015)
            ;;
        xcode)
            # WTF???
            # https://github.com/biojppm/rapidyaml/pull/97/checks?check_run_id=1504677928#step:7:964
            # https://stackoverflow.com/questions/51153525/xcode-10-unable-to-attach-db-error
            echo "-UseModernBuildSystem=NO"
            ;;
        *g++*|*gcc*|*clang*|em++)
            ;;
        "") # allow empty compiler
            ;;
        *)
            echo "unknown compiler"
            exit 1
            ;;
    esac
}
