set(CMAKE_SYSTEM_NAME Linux)
set(CROSS_COMPILER_PREFIX /usr)
set(CROSS_COMPILER_TARGET powerpc64le-linux-gnu)
set(CMAKE_SYSTEM_PROCESSOR powerpc64le)
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-ppc64le-static)
set(CMAKE_C_COMPILER ${CROSS_COMPILER_PREFIX}/bin/${CROSS_COMPILER_TARGET}-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILER_PREFIX}/bin/${CROSS_COMPILER_TARGET}-g++)
set(CMAKE_AR ${CROSS_COMPILER_PREFIX}/bin/${CROSS_COMPILER_TARGET}-ar CACHE FILEPATH "Archiver")
set(CMAKE_MAKE_PROGRAM ${CROSS_COMPILER_PREFIX}/bin/make CACHE FILEPATH "make")

# use -static to avoid having to install the dynamic loader
# see https://mfo.dev.br/2018/03/17/ppc64le-on-x86_64-cross-compilers.html
set(CMAKE_CXX_FLAGS -static)
set(CMAKE_C_FLAGS -static)

set(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILER_PREFIX}/${CROSS_COMPILER_TARGET})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# needed ubuntu packages:
#
# sudo apt-get install -y \
#    gcc-powerpc64le-linux-gnu \
#    g++-powerpc64le-linux-gnu \
#    qemu-user-system
