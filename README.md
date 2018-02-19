# MPXK on Linux v4.12

This repository contains the Linux v4.12 source code with MPXK patches from
[the Towards Kernel Memory Safetey paper](https://arxiv.org/abs/1710.06175)
applied. The Linux fork is based on the [mainline v4.12
kernel](https://github.com/torvalds/linux/tree/v4.12).

## Performance measurements

The setup used for MPXK performance measurements can be found in a [separate
mpxk-perf
branch](https://github.com/ssg-kernel-memory-safety/linux-mpxk/tree/mpxk-perf).

## MPXK plugin

The GCC-plugin is also provided in [stand-alone
format](https://github.com/ssg-kernel-memory-safety/gcc-plugin.mpxk), which
also allows compiling and testing the plugin without the complete Linux kernel
source code.
