#!/bin/sh
cmake --build out -t tests
if [ $? -eq 0 ]; then
    out/tests/tests
else
    echo BUILD FAILED!
fi
