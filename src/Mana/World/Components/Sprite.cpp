#include "Sprite.h"

#include "Mana/Core/Application.h"
#include "Mana/World/Core.h"
#include "Mana/World/ECS.h"

namespace Mana {
SpriteRendering::SpriteRendering(flecs::world &world) {
    // Register Components
    using Components = ECSReflect::ComponentRegistrator<
        TypesList<Sprite, SpriteScale, PPSpriteOutline>>;
    Components::RegisterAll(world);

    // Sprite scale
    world.observer<const SpriteScale, const Sprite>("SpriteScale: UpdateScale")
        .event(flecs::OnSet)
        .each([](flecs::entity e, const SpriteScale &ss, const Sprite &s) {
            float wh = (float)s.Texture->GetWidth() / s.Texture->GetHeight();

            if (wh > 1.0f)
                e.set<Mana::Scale>({ss.Scale * wh, ss.Scale, 1});
            else
                e.set<Mana::Scale>({ss.Scale, ss.Scale / wh, 1});
        });

    world.observer<SpriteScale>("SpriteScale: Add")
        .event(flecs::OnAdd)
        .each([](flecs::entity e, SpriteScale &ss) {
            M_ASSERT(e.has<Sprite>());
            M_ASSERT(e.has<Scale>());

            auto &ws = e.get_mut<Scale>();
            auto &s = e.get<Sprite>();

            float wh = (float)s.Texture->GetWidth() / s.Texture->GetHeight();
            if (wh > 1.0f) {
                ws.y = ss.Scale;
                ws.x = ss.Scale * wh;
            } else {
                ws.x = ss.Scale;
                ws.y = ss.Scale / wh;
            }

            e.modified<Scale>();
        });

    // SpriteRendering
    world
        .system<const Sprite, const WorldPosition, const WorldScale,
                const WorldRotation>("Render Sprites")
        .kind(ECS::Phases.OnRender)
        .each([](flecs::entity e, const Sprite &s, const WorldPosition &wp,
                 const WorldScale &ws, const WorldRotation &r) {
            Application::Instance()
                ->GetGraphicsPipeline()
                .FrameData()
                .Sprites.emplace_back(s.Layer, wp, ws, s.Texture.get(), s.Tint,
                                      r.z);
        });
    world
        .system<const Sprite, const WorldPosition, const WorldScale,
                const WorldRotation>("Render PPSpriteOutline")
        .with<PPSpriteOutline>()
        .kind(ECS::Phases.OnRender)
        .each([](flecs::entity e, const Sprite &s, const WorldPosition &wp,
                 const WorldScale &ws, const WorldRotation &r) {
            Application::Instance()
                ->GetGraphicsPipeline()
                .FrameData()
                .PPSpriteOutline.emplace_back(s.Layer, wp, ws, s.Texture.get(),
                                              s.Tint, r.z);
        });
}
} // namespace Mana
