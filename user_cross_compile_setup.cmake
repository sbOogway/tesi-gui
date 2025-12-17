# Usage:
# cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
# make  -C build -j



message("$ENV{TOOLS}")
message("$ENV{STAGE}")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT $ENV{STAGE})

# set(toolz /home/oogway/amel/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-)
set(CMAKE_C_COMPILER $ENV{TOOLS}gcc)
set(CMAKE_CXX_COMPILER $ENV{TOOLS}g++)


set(COMMON_CONTROL_LIBRARIES $ENV{STAGE}/usr/lib/libcommon-control.so)
set(COMMON_CONTROL_INCLUDE_DIRS $ENV{STAGE}/usr/include/common-control/)

set(EVDEV_INCLUDE_DIRS $ENV{STAGE}/usr/include/libevdev-1.0/)
set(EVDEV_LIBRARIES $ENV{STAGE}/usr/lib/libevdev.so)
# Configure pkg-config for cross-compilation
set(ENV{PKG_CONFIG_SYSROOT_DIR} "$ENV{STAGE}")
set(ENV{PKG_CONFIG_PATH} "$ENV{STAGE}/usr/lib/pkgconfig:$ENV{STAGE}/usr/share/pkgconfig")

# Configure CMake find commands for cross-compilation
set(CMAKE_FIND_ROOT_PATH "$ENV{STAGE}")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# If necessary, set STAGING_DIR
# if not work, please try(in shell command): export STAGING_DIR=/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target
# set(ENV{STAGING_DIR} "/home/oogway/amel/buildroot/output/staging")

# set(CMAKE_SYSROOT /home/oogway/amel/buildroot/output/staging)

