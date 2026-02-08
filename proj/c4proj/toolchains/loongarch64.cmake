set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR loongarch64)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/loongarch64-linux-gnu-gcc-14)
set(CMAKE_CXX_COMPILER /usr/bin/loongarch64-linux-gnu-g++-14)

set(CMAKE_FIND_ROOT_PATH /usr/loongarch64-linux-gnu/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-loongarch64-static;-L;/usr/loongarch64-linux-gnu/" CACHE FILEPATH "Path to the emulator for the target system.")
