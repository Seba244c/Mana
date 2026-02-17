#include <Mana.h>
#include <gtest/gtest.h>

TEST(Input, KeyPresses) {
    Mana::Input::NewFrame();
    EXPECT_FALSE(Mana::Input::IsKeyPressed(Mana::Key::F10));
    EXPECT_FALSE(Mana::Input::IsKeyDown(Mana::Key::F10));

    Mana::Input::NewFrame();
    Mana::Event *event = new Mana::KeyPressedEvent{Mana::Key::F10};
    Mana::Input::Event(*event);
    delete (event);
    EXPECT_TRUE(Mana::Input::IsKeyPressed(Mana::Key::F10));
    EXPECT_TRUE(Mana::Input::IsKeyDown(Mana::Key::F10));

    Mana::Input::NewFrame();
    EXPECT_FALSE(Mana::Input::IsKeyPressed(Mana::Key::F10));
    EXPECT_TRUE(Mana::Input::IsKeyDown(Mana::Key::F10));

    Mana::Input::NewFrame();
    EXPECT_FALSE(Mana::Input::IsKeyPressed(Mana::Key::F10));
    EXPECT_TRUE(Mana::Input::IsKeyDown(Mana::Key::F10));

    Mana::Input::NewFrame();
    event = new Mana::KeyReleasedEvent{Mana::Key::F10};
    Mana::Input::Event(*event);
    delete (event);
    EXPECT_FALSE(Mana::Input::IsKeyPressed(Mana::Key::F10));
    EXPECT_FALSE(Mana::Input::IsKeyDown(Mana::Key::F10));
}

TEST(Input, MouseClicks) {
    Mana::Input::NewFrame();
    EXPECT_FALSE(Mana::Input::IsMouseButtonDown(Mana::Mouse::Button1));
    EXPECT_FALSE(Mana::Input::IsMouseButtonPressed(Mana::Mouse::Button1));

    Mana::Input::NewFrame();
    Mana::Event *event = new Mana::ButtonPressedEvent{Mana::Mouse::Button1};
    Mana::Input::Event(*event);
    delete (event);
    EXPECT_TRUE(Mana::Input::IsMouseButtonDown(Mana::Mouse::Button1));
    EXPECT_TRUE(Mana::Input::IsMouseButtonPressed(Mana::Mouse::Button1));

    Mana::Input::NewFrame();
    EXPECT_TRUE(Mana::Input::IsMouseButtonDown(Mana::Mouse::Button1));
    EXPECT_FALSE(Mana::Input::IsMouseButtonPressed(Mana::Mouse::Button1));

    Mana::Input::NewFrame();
    EXPECT_TRUE(Mana::Input::IsMouseButtonDown(Mana::Mouse::Button1));
    EXPECT_FALSE(Mana::Input::IsMouseButtonPressed(Mana::Mouse::Button1));

    Mana::Input::NewFrame();
    event = new Mana::ButtonReleasedEvent{Mana::Mouse::Button1};
    Mana::Input::Event(*event);
    delete (event);
    EXPECT_FALSE(Mana::Input::IsMouseButtonDown(Mana::Mouse::Button1));
    EXPECT_FALSE(Mana::Input::IsMouseButtonPressed(Mana::Mouse::Button1));
}

void Press(Mana::Key::KeyCode keycode) {
    Mana::Event *event = new Mana::KeyPressedEvent{keycode};
    Mana::Input::Event(*event);
    delete (event);
}

void Release(Mana::Key::KeyCode keycode) {
    Mana::Event *event = new Mana::KeyReleasedEvent{keycode};
    Mana::Input::Event(*event);
    delete (event);
}

TEST(Input, InputVectorKeys) {
    Mana::Input::NewFrame();
    auto vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S,
                                        Mana::Key::A, Mana::Key::D);
    EXPECT_FLOAT_EQ(length(vec), 0);

    // All directions are correct
    Mana::Input::NewFrame();
    Press(Mana::Key::W);
    vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S, Mana::Key::A,
                                   Mana::Key::D);
    EXPECT_FLOAT_EQ(vec.x, 0);
    EXPECT_FLOAT_EQ(vec.y, 1);

    Mana::Input::NewFrame();
    Release(Mana::Key::W);
    Press(Mana::Key::S);
    vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S, Mana::Key::A,
                                   Mana::Key::D);
    EXPECT_FLOAT_EQ(vec.x, 0);
    EXPECT_FLOAT_EQ(vec.y, -1);

    Mana::Input::NewFrame();
    Release(Mana::Key::S);
    Press(Mana::Key::A);
    vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S, Mana::Key::A,
                                   Mana::Key::D);
    EXPECT_FLOAT_EQ(vec.x, -1);
    EXPECT_FLOAT_EQ(vec.y, 0);

    Mana::Input::NewFrame();
    Release(Mana::Key::A);
    Press(Mana::Key::D);
    vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S, Mana::Key::A,
                                   Mana::Key::D);
    EXPECT_FLOAT_EQ(vec.x, 1);
    EXPECT_FLOAT_EQ(vec.y, 0);

    // A diagonal
    Mana::Input::NewFrame();
    Press(Mana::Key::W);
    vec = Mana::Input::InputVector(Mana::Key::W, Mana::Key::S, Mana::Key::A,
                                   Mana::Key::D);
    EXPECT_FLOAT_EQ(length(vec), 1);
    EXPECT_FLOAT_EQ(vec.x, vec.y);
}
