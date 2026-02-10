# Building and Running Spellrite
## Step 1. Installing neccesary dependencies
> If you find anything missing from here, create an issue or a PR

**Nix & NixOS** Run `nix develop` and you should be golden
### All Systems
You must have installed `Python3`, `CMake`, `ninja` and `vcpkg` to be able to use the tooling around Mana and Spellrite. Further down this document will be more dependencies depending on you operating system.

### Unix like systems
We only support using the LLVM / clang toolchain

**Ubuntu** `sudo apt install libwayland-dev libxkbcommon-dev glslc libpulse-dev clang`

**Fedora / RHEL** `sudo dnf install wayland-devel libxkbcommon-devel glslc pulseaudio-libs-devel clang`

### OSX
Supported documentation coming soon

### Windows
Untested documentation coming later

## Step 2. Configuring CMake
The tools `scripts/` are gonna be your go to way to configure, build and run the project. To make the project ready to run call `python3 scripts/configure.py` from the root directory, and select your buildmode, architecture and OS

## Step 3. Building and running
You can run `python3 scripts/build.py` to build the executable, or you can run `python3 scripts/run.py` to both build and run the executable.
