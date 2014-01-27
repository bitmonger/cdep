cdep
====

A simple dependency resolver with a .ini file format

There are many dependency resolving tools, but they can be very complicated,
and perhaps do more than in necessary.  This tool is designed to be the
simplest way possible to generate a build order for a project.  

A separate tool is being developed which will integrate with git, and create a
more complete build system.

Example:

deps.ini:

    [userspace app]
    cmd = ./userspace/build.sh
    dep = kernel

    [kernel]
    cmd = make -C kernel defconfig
    cmd = make -C kernel bzImage -j16
    dep = toolchain

    [bootloader]
    cmd = ./bootloader/build.sh
    dep = toolchain

    [toolchain]
    cmd = ./toolchain/build.sh

Run cdep:
    
    $ cdep deps.ini
    1 "toolchain" "./toolchain/build.sh"
    2 "kernel" "make -C kernel defconfig"
    2 "kernel" "make -C kernel bzImage -j16"
    2 "bootloader" "./bootloader/build.sh"
    3 "userspace app" "./userspace/build.sh"

