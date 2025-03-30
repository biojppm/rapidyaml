set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/mips-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/mips-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/mips-linux-gnu/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-mips-static;-L;/usr/mips-linux-gnu/" CACHE FILEPATH "Path to the emulator for the target system.")
