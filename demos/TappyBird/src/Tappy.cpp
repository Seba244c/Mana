#include "Mana/Core/Layer.h"
#include "Mana/IO/Input.h"
#include "Mana/IO/KeyCodes.h"
#include "Mana/Math/Color.h"
#include "Mana/Math/Random.h"
#include "Mana/Physics/Physics.h"
#include "Mana/Physics/Physics2D.h"
#include "Mana/Rendering/Cameras/Orthographic.h"
#include "Mana/World/Core.h"
#include "Mana/World/ECS.h"

#include "Components/Player.hpp"
#include "Components/World.hpp"
#include <Mana.h>
#include <filesystem>
#include <flecs/addons/cpp/c_types.hpp>
#include <flecs/addons/cpp/entity.hpp>

static flecs::entity E_BG, E_Pipes, E_Player;

constexpr const int pipesCount = 4;
static bool GameStarted = false;

class DeathUI final : public Mana::Layer {
    void OnAttach() override {
        m_InterFont = M_ASSET(Mana::Font, "fonts/Inter-Regular.ttf");
    }

    void Update(const Mana::Time::Duration &delta) override {}

    void Draw() override {
        Mana::UI::HorizontalAlign()->FillParent().Children([&]() {
            Mana::UI::Text("YOU DIED!")
                ->Font(m_InterFont)
                .Size(50)
                .Color(Mana::Colors::White)
                .Position({0, Mana::UI::ParentSize().y - 100});

            Mana::UI::Text("Press JUMP to play again.")
                ->Font(m_InterFont)
                .Size(24)
                .Color(Mana::Colors::White)
                .Position({0, Mana::UI::ParentSize().y - 124});
        });
    }

  private:
    Mana::Ref<Mana::Font> m_InterFont;
};

class StartUI final : public Mana::Layer {
    void OnAttach() override {
        m_InterFont = M_ASSET(Mana::Font, "fonts/Inter-Regular.ttf");
    }

    void Update(const Mana::Time::Duration &delta) override {}

    void Draw() override {
        Mana::UI::HorizontalAlign()->FillParent().Children([&]() {
            Mana::UI::Text("Tappy Bird")
                ->Font(m_InterFont)
                .Size(50)
                .Color(Mana::Colors::White)
                .Position({0, Mana::UI::ParentSize().y - 100});

            Mana::UI::Text("Press JUMP to start!")
                ->Font(m_InterFont)
                .Size(24)
                .Color(Mana::Colors::White)
                .Position({0, Mana::UI::ParentSize().y - 124});
        });
    }

  private:
    Mana::Ref<Mana::Font> m_InterFont;
};

static DeathUI *s_LayerDeath;
static StartUI *s_LayerStart;
static Mana::Layerstack s_Layerstack;

void StartGame() {
    M_INFO("START!");
    s_Layerstack.PopLayer(s_LayerDeath);
    auto e = s_Layerstack.PopLayer(s_LayerStart);
    GameStarted = true;

    E_BG.enable<Game::BackgroundManager>();

    float x = 30;
    E_Pipes.enable<Game::PipeManager>();
    E_Pipes.children([&x](flecs::entity c) {
        c.set<Mana::Position>(
            {x,
             Mana::Random::Float(-Game::PipeManager::SpawnRange,
                                 Game::PipeManager::SpawnRange),
             0});
        x += 13;
    });

    E_Player.set<Mana::Position>({0, 0, 0});
    E_Player.get_mut<Game::TappyController>().Velocity =
        Game::TappyController::JUMP_ENERGY;
    E_Player.enable<Game::TappyController>();
    E_Player.world().get_mut<Game::Score>().Score = 0;
}

void StopGame() {
    M_INFO("STOP!");
    GameStarted = false;
    E_Player.disable<Game::TappyController>();
    E_Pipes.disable<Game::PipeManager>();
    E_BG.disable<Game::BackgroundManager>();
    s_Layerstack.PushLayer(s_LayerDeath);
}

class UILayer final : public Mana::Layer {
    void OnAttach() override {
        m_InterFont = M_ASSET(Mana::Font, "fonts/Inter-Regular.ttf");
    }

    void Update(const Mana::Time::Duration &delta) override { Dirty = true; }

    void Draw() override {
        Mana::UI::HorizontalAlign()->FillParent().Children([&]() {
            Mana::UI::Text(std::format("{}", Mana::Application::Instance()
                                                 ->World()
                                                 .get<Game::Score>()
                                                 .Score))
                ->Font(m_InterFont)
                .Size(50)
                .Color(Mana::Colors::White)
                .Position({0, 50});
        });
    }

  private:
    Mana::Ref<Mana::Font> m_InterFont;
};

class Tappy final : public Mana::Application {
    const Mana::AppInfo GetAppInfo() override {
        m_AppConfig.Rendering_ClearColor = Mana::Colors::Red;
        m_AppConfig.Assets_GarbageCollection = false;

        // Find dir we are running from
        std::filesystem::path testPath("./Assets/");
        if (std::filesystem::is_directory(testPath)) {
            M_TRACE("Adding asset directory relative to demo");
            m_AppConfig.Assets_SourceFolders.push_back("Assets/");
        } else {
            M_TRACE("Adding asset directory relative to root");
            m_AppConfig.Assets_SourceFolders.push_back(
                "./demos/TappyBird/Assets/");
        }

        return Mana::AppInfo{"TappyBird", "ssnoer", "1.0.0", "git-latest"};
    }

    void OnStartup() override {
        Game::InitAudio();
        Mana::ECSReflect::ComponentRegistrator<Mana::TypesList<
            Game::TappyController, Game::BackgroundManager, Game::PipeManager>>
            components;
        components.RegisterAll(m_World);
        Game::TappyController::Register(m_World);
        Game::BackgroundManager::RegisterSystem(m_World);
        Game::PipeManager::RegisterSystem(m_World);

        E_Player = m_World.entity("Tappy")
                       .set(Mana::Scale{1.7f * 0.7f, 1.2f * 0.7f, 0})
                       .add<Mana::BoxCollider>()
                       .add<Game::TappyController>()
                       .add<Mana::RigidBody2D>();
        m_World
            .entity("TappySprite") // We want the collider to be smaller than
                                   // the sprite, we do this by making the
                                   // sprite bigger than xD
            .child_of(E_Player)
            .set(Mana::Scale{1.4f, 1.4f, 0})
            .set(Mana::Sprite{M_ASSET(Mana::Texture, "tappy.png"),
                              Mana::Colors::White, 10});

        E_Player.observe<Mana::CollisionEvent>([&](Mana::CollisionEvent &c) {
            if (c.Other.has<Game::Death>()) {
                M_INFO("Played died");
                StopGame();

            } else if (c.Other.has<Game::Point>()) {
                if (!c.Other.get<Game::Point>().Scored) {
                    c.Other.set<Game::Point>({true});
                    m_World.get_mut<Game::Score>().Score++;
                }
            }
        });

        m_World.entity("Camera").set(Mana::OrthoCamera((16.0f / 9.0f), 10, 0));

        constexpr const int bgCount = 4;
        E_BG = m_World.entity("BackgroundManager")
                   .set(Game::BackgroundManager{.Width = 22, .Count = bgCount});

        float x = -11 * bgCount * 0.5f;
        for (int i = 0; i < bgCount; i++) {
            m_World.entity()
                .child_of(E_BG)
                .set(Mana::Position{x, 0, 0})
                .set(Mana::Scale{22.1f, 20, 0})
                .set(Mana::Sprite{M_ASSET(Mana::Texture, "background.png"),
                                  Mana::Colors::White});
            x += 22;
        }

        E_Pipes = m_World.entity("PipeManager")
                      .set(Game::PipeManager{.Width = 13, .Count = pipesCount});

        x = 30;
        for (int i = 0; i < pipesCount; i++) {
            Game::PPipe(m_World, E_Pipes, x);
            x += 13;
        }

        // Add bounds colliders (they are constant)
        m_World.entity("Collider Top")
            .set(Mana::Scale(40, 2, 0))
            .set(Mana::Position(0, 11, 0))
            .add<Mana::BoxCollider>();

        m_World.entity("Collider Bot")
            .set(Mana::Scale(40, 2, 0))
            .set(Mana::Position(0, -10, 0))
            .add<Game::Death>()
            .add<Mana::BoxCollider>();
        m_World.add<Game::Score>();

        // Add Start component
        m_World.system("StartOnTap")
            .kind(Mana::ECS::Phases.OnUpdate)
            .run([](flecs::iter &it) {
                if (!GameStarted && Mana::Input::IsKeyPressed(Mana::Key::SPACE))
                    StartGame();
            });
        E_Player.disable<Game::TappyController>();
        E_Pipes.disable<Game::PipeManager>();
        s_LayerStart = new StartUI;
        s_LayerDeath = new DeathUI;
        m_Layerstack.PushLayer(new UILayer);
        m_Layerstack.PushLayer(s_LayerStart);
        s_Layerstack = m_Layerstack;
    }

    void OnClose() override {}
};

int main(int argc, char *argv[]) { return Mana::StartApplication(new Tappy()); }
