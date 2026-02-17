configure mode="Debug":
    @echo 'Configuring CMake with {{mode}} mode'
    @echo "Toolchain: {{arch()}}-{{os_family()}}-llvm"
    cmake \
        -S . \
        -B out \
        -G Ninja \
        --toolchain cmake/{{arch()}}-{{os_family()}}-llvm.cmake \
        -DCMAKE_BUILD_TYPE={{mode}} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1

    # Link compile_commands which is needed for some IDE's
    ln -fs out/compile_commands.json .

build:
    cmake --build out

test: build
    out/tests/tests

run_demo: build
    out/demos/TappyBird/TappyBird

