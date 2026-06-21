set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mipsel)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/mipsel-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/mipsel-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/mipsel-linux-gnu/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-mipsel-static;-L;/usr/mipsel-linux-gnu/" CACHE FILEPATH "Path to the emulator for the target system.")
