add_executable(TappyBird src/Tappy.cpp)

# Add Mana
target_link_libraries(TappyBird LINK_PRIVATE Mana)

