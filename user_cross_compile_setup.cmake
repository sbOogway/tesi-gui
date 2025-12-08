# Usage:
# cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
# make  -C build -j

# set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR arm)

# set(tools /home/oogway/amel/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-)

# set(staging_dir /home/oogway/amel/buildroot/output/staging)

# set(tools $ENV{TOOLS})
# set(STAGING_DIR $ENV{STAGING_DIR})

# Cross compilers
# set(CMAKE_C_COMPILER ${tools}gcc)
# set(CMAKE_CXX_COMPILER ${tools}g++)
# CRITICAL: Set sysroot for cross-compilation
# set(CMAKE_SYSROOT ${staging_dir})
# Find root path for library/header discovery
# set(CMAKE_FIND_ROOT_PATH ${staging_dir})
# Search modes for cross-compilation
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
# pkg-config environment for cross-compilation
# set(ENV{PKG_CONFIG_SYSROOT_DIR} ${staging_dir})
# set(ENV{PKG_CONFIG_LIBDIR} "${staging_dir}/usr/lib/pkgconfig:${staging_dir}/usr/share/pkgconfig")
# set(ENV{PKG_CONFIG_DIR} "")
# # Library architecture
# set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)

# set(CMAKE_C_COMPILER ${tools}gcc)
# set(CMAKE_CXX_COMPILER ${tools}g++)


# set(EVDEV_INCLUDE_DIRS /home/oogway/amel/buildroot/output/staging/usr/include/libevdev-1.0/)
# set(EVDEV_LIBRARIES /home/oogway/amel/buildroot/output/staging/usr/lib/libevdev.so)

# set(BUILD_SHARED_LIBS ON)


# If necessary, set STAGING_DIR
# if not work, please try(in shell command): export STAGING_DIR=/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target
# set(ENV{STAGING_DIR} "/home/oogway/amel/buildroot/output/staging")

# set(CMAKE_SYSROOT /home/oogway/amel/buildroot/output/staging)

# set(CMAKE_C_COMPILER_ID GNU)  #Add these
# set(CMAKE_CXX_COMPILER_ID GNU)

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

