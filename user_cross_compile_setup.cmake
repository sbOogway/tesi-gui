# Usage:
# cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
# make  -C build -j

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(tools /home/oogway/amel/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-)
set(CMAKE_C_COMPILER ${tools}gcc)
set(CMAKE_CXX_COMPILER ${tools}g++)

set(EVDEV_INCLUDE_DIRS /home/oogway/amel/buildroot/output/staging/usr/include/libevdev-1.0/)
set(EVDEV_LIBRARIES /home/oogway/amel/buildroot/output/staging/usr/lib/libevdev.so)

# If necessary, set STAGING_DIR
# if not work, please try(in shell command): export STAGING_DIR=/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target
# set(ENV{STAGING_DIR} "/home/oogway/amel/buildroot/output/staging")

# set(CMAKE_SYSROOT /home/oogway/amel/buildroot/output/staging)

