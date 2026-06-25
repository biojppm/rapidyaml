set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips64)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/mips64-linux-gnuabi64-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/mips64-linux-gnuabi64-g++)

set(CMAKE_FIND_ROOT_PATH /usr/mips64-linux-gnuabi64/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-mips64-static;-L;/usr/mips64-linux-gnuabi64/" CACHE FILEPATH "Path to the emulator for the target system.")
