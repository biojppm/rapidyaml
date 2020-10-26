#!/usr/bin/env bash

set -e
set -x

PROJ_DIR=$(pwd)

function c4_show_info()
{
    set +x
    echo "PROJ_DIR=$PROJ_DIR"
    echo "PROJ_PFX_TARGET=$PROJ_PFX_TARGET"
    echo "PROJ_PFX_CMAKE=$PROJ_PFX_CMAKE"
    echo "CMAKE_FLAGS=$CMAKE_FLAGS"
    echo "NUM_JOBS_BUILD=$NUM_JOBS_BUILD"
    echo "GITHUB_WORKSPACE=$GITHUB_WORKSPACE"
    pwd
    ls -lFhp
    echo "BITLINKS=$BITLINKS"
    for bl in shared64 static64 shared32 static32 ; do
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
    esac
    set -x
}

function _c4bits()
{
    case "$1" in
        shared64|static64) echo 64 ;;
        shared32|static32) echo 32 ;;
        *) exit 1 ;;
    esac
}

function _c4linktype()
{
    case "$1" in
        shared64|shared32) echo shared ;;
        static64|static32) echo static ;;
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
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    build_dir=`pwd`/build/$bits-$linktype   # see c4_cfg_test()
    export CTEST_OUTPUT_ON_FAILURE=1
    # watchout: the `--parallel` flag to `cmake --build` is broken:
    # https://discourse.cmake.org/t/parallel-does-not-really-enable-parallel-compiles-with-msbuild/964/10
    # https://gitlab.kitware.com/cmake/cmake/-/issues/20564
    cmake --build $build_dir --target test-build -- $(_c4_parallel_build_flags)
}

function c4_run_test()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    build_dir=`pwd`/build/$bits-$linktype   # see c4_cfg_test()
    export CTEST_OUTPUT_ON_FAILURE=1
    cmake --build $build_dir --target test
}

function c4_pack()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    build_dir=`pwd`/build/$bits-$linktype   # see c4_cfg_test()
    cmake --build $build_dir --target package
}

function c4_submit_coverage()
{
    if [ "$BT" != "Coverage" ] ; then
        echo "build type is \"$BT\": no coverage to submit"
        return 0
    fi
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    coverage_service=$2
    build_dir=`pwd`/build/$bits-$linktype   # see c4_cfg_test()
    if [ "$CXX_" == "xcode" ] && [ "$bits" == "32" ] ; then return 0 ; fi
    echo "Submitting coverage data: $build_dir --> $coverage_service"
    cmake --build $build_dir --target ${PROJ_PFX_TARGET}coverage-submit-$coverage_service
}

function c4_run_static_analysis()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    build_dir=`pwd`/build/$bits-$linktype   # see c4_cfg_test()
    # https://blog.kitware.com/static-checks-with-cmake-cdash-iwyu-clang-tidy-lwyu-cpplint-and-cppcheck/
    pushd $PROJ_DIR
}

function c4_cfg_test()
{
    if _c4skipbitlink "$1" ; then return 0 ; fi
    bits=$(_c4bits $1)
    linktype=$(_c4linktype $1)
    #
    build_dir=`pwd`/build/$bits-$linktype
    install_dir=`pwd`/install/$bits-$linktype
    mkdir -p $build_dir
    mkdir -p $install_dir
    #
    case "$linktype" in
        static) _addcmkflags -DBUILD_SHARED_LIBS=OFF ;;
        shared) _addcmkflags -DBUILD_SHARED_LIBS=ON ;;
        *)
            echo "ERROR: unknown linktype: $linktype"
            exit 1
            ;;
    esac
    if [ "$STD" != "" ] ; then
        _addcmkflags -DC4_CXX_STANDARD=$STD
        _addprojflags CXX_STANDARD=$STD
    fi
    #
    _addprojflags DEV=ON
    if [ ! -z "$CMAKE_FLAGS" ] ; then
        _addcmkflags $CMAKE_FLAGS
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
        _addprojflags COVERAGE_CODECOV=ON COVERAGE_CODECOV_SILENT=ON
        _addprojflags COVERAGE_COVERALLS=ON COVERAGE_COVERALLS_SILENT=ON
    fi
    _addcmkflags -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

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
        vs2019)
            g='Visual Studio 16 2019'
            case "$bits" in
                64) a=x64 ;;
                32) a=Win32 ;;
            esac
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT -G "$g" -A $a $CMFLAGS
            ;;
        vs2017)
            case "$bits" in
                64) g="Visual Studio 15 2017 Win64" ;;
                32) g="Visual Studio 15 2017" ;;
            esac
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT -G "$g" $CMFLAGS
            ;;
        xcode)
            g=Xcode
            case "$bits" in
                64) a="x86_64" ;;
                32) a="i386"
                    exit 1 # i386 is deprecated in xcode
                    ;;
            esac
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT -G "$g" -DCMAKE_OSX_ARCHITECTURES=$a $CMFLAGS
            ;;
        *g++*|*gcc*|*clang*)
            export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')
            _c4_choose_clang_tidy $CXX_
            cmake -S $PROJ_DIR -B $build_dir -DCMAKE_INSTALL_PREFIX="$install_dir" \
                  -DCMAKE_BUILD_TYPE=$BT $CMFLAGS \
                  -DCMAKE_C_COMPILER=$CC_ -DCMAKE_CXX_COMPILER=$CXX_ \
                  -DCMAKE_C_FLAGS="-std=c99 -m$bits" -DCMAKE_CXX_FLAGS="-m$bits"
            ;;
        "") # allow empty compiler
            ;;
        *)
            echo "unknown compiler"
            exit 1
            ;;
    esac
    cmake --build $build_dir --target help | sed 1d | sort
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

function _c4_parallel_build_flags()
{
    case "$CXX_" in
        vs2019|vs2017|vs2015)
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
        *g++*|*gcc*|*clang*)
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
