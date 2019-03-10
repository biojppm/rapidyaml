#!/usr/bin/env bash

set -e
set -x

# setup apt for installing
#sudo -E add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo bash -c 'cat >> /etc/apt/sources.list <<EOF

deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main
#deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.8 main # not needed
deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.9 main
deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-4.0 main
deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-5.0 main
deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-6.0 main
#deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-7.0 main
# Also add the following for the appropriate libstdc++
#deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu trusty main
EOF'

#cat /etc/apt/sources.list
#cat /etc/apt/sources.list.d/ubuntu-toolchain-r-test-trusty.list
#sed -n '33p' /etc/apt/sources.list
#sed -n '1p' /etc/apt/sources.list.d/ubuntu-toolchain-r-test-trusty.list

sudo apt-get clean
sudo -E apt-get update

  if [ "$CXX_" == "g++-4.9" ] && [ "$A" == "64" ] ; then DPKG="$DPKG g++-4.9"
elif [ "$CXX_" == "g++-4.9" ] && [ "$A" == "32" ] ; then DPKG="$DPKG g++-4.9 g++-4.9-multilib"
elif [ "$CXX_" == "g++-5"   ] && [ "$A" == "64" ] ; then DPKG="$DPKG"
elif [ "$CXX_" == "g++-5"   ] && [ "$A" == "32" ] ; then DPKG="$DPKG"
elif [ "$CXX_" == "g++-6"   ] && [ "$A" == "64" ] ; then DPKG="$DPKG g++-6"
elif [ "$CXX_" == "g++-6"   ] && [ "$A" == "32" ] ; then DPKG="$DPKG g++-6 g++-6-multilib"
elif [ "$CXX_" == "g++-7"   ] && [ "$A" == "64" ] ; then DPKG="$DPKG g++-7"
elif [ "$CXX_" == "g++-7"   ] && [ "$A" == "32" ] ; then DPKG="$DPKG g++-7 g++-7-multilib"
elif [ "$CXX_" == "g++-8"   ] && [ "$A" == "64" ] ; then DPKG="$DPKG g++-8"
elif [ "$CXX_" == "g++-8"   ] && [ "$A" == "32" ] ; then DPKG="$DPKG g++-8 g++-8-multilib"
elif [ "$CXX_" == "clang++-3.6" ] ; then DPKG="$DPKG clang-3.6"
elif [ "$CXX_" == "clang++-3.7" ] ; then DPKG="$DPKG clang-3.7"
elif [ "$CXX_" == "clang++-3.8" ] ; then DPKG="$DPKG clang-3.8"
elif [ "$CXX_" == "clang++-3.9" ] ; then DPKG="$DPKG clang-3.9"
elif [ "$CXX_" == "clang++-4.0" ] ; then DPKG="$DPKG clang-4.0"
elif [ "$CXX_" == "clang++-5.0" ] ; then DPKG="$DPKG clang-5.0"
elif [ "$CXX_" == "clang++-6.0" ] ; then DPKG="$DPKG clang-6.0"
elif [ "$CXX_" == "clang++-7" ] ; then DPKG="$DPKG clang-7"
elif [ "$CXX_" == "all" ] ; then
    DPKG="$DPKG \
g++-6 g++-6-multilib \
g++-7 g++-7-multilib \
clang-3.6 \
clang-3.7 \
clang-3.8 \
clang-3.9 \
clang-4.0 \
clang-5.0 \
clang-6.0 \
clang-7 \
"
fi

if [ "$A" == "32" ] ; then
    DPKG="$DPKG libc6-dbg:i386"
fi

if [ "$BT" == "Coverage" ] ; then
    DPKG="$DPKG lcov libffi-dev libssl-dev"
fi

if [ ! -z "$DPKG" ] ; then
    echo "additional packages: $DPKG"
fi

# g++-5 is needed for clang; otherwise it uses libstdc++ from g++4.8
# which is not fully C++11 and does not have some important type traits
# such as std::is_trivially_move_constructible.
sudo -E apt-get install -y --force-yes \
     build-essential \
     g++-5 g++-5-multilib \
     valgrind \
     $DPKG

if [ "$BT" == "Coverage" ]; then
    export PATH=~/.local/bin:${PATH};
    pip install --user --upgrade pip;
    # https://github.com/pypa/pip/issues/5221#issuecomment-381568428
    # (via https://github.com/pypa/pip/issues/5240)
    hash -d pip || hash -d $(which pip) || echo "WTF???"
    # https://stackoverflow.com/questions/29134512/insecureplatformwarning-a-true-sslcontext-object-is-not-available-this-prevent
    pip install --user requests[security] pyopenssl ndg-httpsclient pyasn1
    pip install --user cpp-coveralls;
fi

export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')
$CXX_ --version
$CC_ --version
cmake --version

RYML_DIR=$(pwd)
echo $RYML_DIR
pwd

exit 0
