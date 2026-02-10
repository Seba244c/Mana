#!/bin/sh
cmake \
    -S . \
    -B out \
    -G Ninja \
    --toolchain cmake/x64-linux-llvm.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1

ln -fs out/compile_commands.json .
