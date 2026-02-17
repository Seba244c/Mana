set(CMAKE_SYSTEM_NAME Darwin)

# Compilers
set(CMAKE_C_COMPILER clang CACHE STRING "")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "")

# Linker
set(CMAKE_LINKER_TYPE LLD CACHE STRING "")
set(CMAKE_LINKER ld64.lld CACHE STRING "")
set(CMAKE_C_LINKER ld64.lld CACHE STRING "")
set(CMAKE_CXX_LINKER ld64.lld CACHE STRING "")

# libc++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi -L/opt/homebrew/opt/llvm/lib/c++ -L/opt/homebrew/opt/llvm/lib/unwind -lunwind" CACHE STRING "")
