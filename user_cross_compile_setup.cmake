# Usage:
# cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
# make  -C build -j



message("$ENV{TOOLS}")
message("$ENV{STAGE}")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(toolz /home/oogway/amel/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-)
set(CMAKE_C_COMPILER $ENV{TOOLS}gcc)
set(CMAKE_CXX_COMPILER $ENV{TOOLS}g++)

set(EVDEV_INCLUDE_DIRS $ENV{STAGING_DIR}/usr/include/libevdev-1.0/)
set(EVDEV_LIBRARIES $ENV{STAGING_DIR}/usr/lib/libevdev.so)

# If necessary, set STAGING_DIR
# if not work, please try(in shell command): export STAGING_DIR=/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target
# set(ENV{STAGING_DIR} "/home/oogway/amel/buildroot/output/staging")

# set(CMAKE_SYSROOT /home/oogway/amel/buildroot/output/staging)

