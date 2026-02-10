cmake_minimum_required(VERSION 3.28)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723) # 1.17.0
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests "src/Math.cpp")
target_link_libraries(tests PRIVATE GTest::gtest_main Mana)
include(GoogleTest)
gtest_discover_tests(tests)
