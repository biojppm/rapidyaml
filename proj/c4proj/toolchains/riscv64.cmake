set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/riscv64-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/riscv64-linux-gnu/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-riscv64-static;-L;/usr/riscv64-linux-gnu/" CACHE FILEPATH "Path to the emulator for the target system.")
