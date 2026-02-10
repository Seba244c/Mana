#!/bin/sh
cmake --build out -t TappyBird
if [ $? -eq 0 ]; then
    out/demos/TappyBird/TappyBird
else
    echo BUILD FAILED!
fi
