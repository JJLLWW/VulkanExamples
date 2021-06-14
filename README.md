# Vulkan Examples

This repository contains example code for the Vulkan API. The examples can only run on 
Windows or Linux.

## Contents

(nothing works yet lol)

## Cloning

**This repository must be cloned with the --recursive flag as it makes use of git submodules.** 

If you have done a raw git clone by accident, pull the submodules with
**git submodule update --init**.

* Fast clone: git clone --recursive --depth 1
* Developer clone: git clone --recursive

Cloning with --depth 1 is significantly faster, but it erases the commit history.

## Prerequisites

* CMake > version 3.13
* Any well known C and C++ toolchain.

## Building

This repository uses CMake to generate build files for other platform specific C++ build 
systems (for example nmake, or Makefiles on Unix).

The scripts/ subdirectory has scripts to automate the build process for the most common 
situations, they should be run from the root of the repository:

* Windows + MSVC: ./scripts/build-windows-msvc.bat
* Linux + Make: ./scripts/build-linux-make.sh

In other cases, typical CMake usage looks like this:

~~~bash
cmake -G "BUILD_SYSTEM_TYPE" PATH_TO_CMAKELISTS
cmake --build . 
~~~

It is usually a smart idea to run these commands in a newly created subdirectory, as CMake
dumps build artifacts into the working directory.    

## License

Licensed under the GPL 3 license.