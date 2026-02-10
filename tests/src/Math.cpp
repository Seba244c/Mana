#include <Mana/Math/Math.h>
#include <gtest/gtest.h>

TEST(Math, PointInRect) {
    EXPECT_TRUE(Mana::Math::PointInRect({0, 0}, {0, 0}, {5, 5}));
    EXPECT_TRUE(Mana::Math::PointInRect({0, 5}, {0, 0}, {5, 5}));
    EXPECT_TRUE(Mana::Math::PointInRect({5, 5}, {0, 0}, {5, 5}));
    EXPECT_TRUE(Mana::Math::PointInRect({5, 0}, {0, 0}, {5, 5}));
    EXPECT_TRUE(Mana::Math::PointInRect({2, 2}, {0, 0}, {5, 5}));
    EXPECT_FALSE(Mana::Math::PointInRect({-1, -1}, {0, 0}, {5, 5}));
    EXPECT_FALSE(Mana::Math::PointInRect({10, -1}, {0, 0}, {5, 5}));
    EXPECT_FALSE(Mana::Math::PointInRect({10, 10}, {0, 0}, {5, 5}));
    EXPECT_FALSE(Mana::Math::PointInRect({-1, 10}, {0, 0}, {5, 5}));
}
