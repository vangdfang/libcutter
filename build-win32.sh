#!/bin/sh
mkdir build-win32
cd build-win32
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain.mingw32 ..
make
