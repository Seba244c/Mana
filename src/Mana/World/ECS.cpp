#include "ECS.h"

#include "Mana/Physics/Physics.h"
#include "Mana/Rendering/Renderer.h"
#include "Mana/World/Components/Sprite.h"
#include "Mana/World/Core.h"

#include "Components/Chunk.h"
#include "Components/Tilemap.h"

namespace Mana {
Phases ECS::Phases;
ECSRegistry ECS::Registry;
void ECS::Init(flecs::world &world) {
    MC_TRACE("Setting up ECS");

// If we are in debug mode, enable support for flecs explorer
#if M_DEBUG_INFO
    MC_INFO("Enabling flecs::Rest api, for flecs explorer. See world at "
            "https://flecs.dev/explorer");
    world.set<flecs::Rest>({});
    world.import <flecs::stats>();
#endif

    // Setup Phases
    Phases.OnValidate = world.entity("Phase OnValidate")
                            .add(flecs::Phase)
                            .depends_on(flecs::OnStore);
    Phases.PostUpdate = world.entity("Phase PostUpdate")
                            .add(flecs::Phase)
                            .depends_on(Phases.OnValidate);
    Phases.PreRender = world.entity("Phase PreRender")
                           .add(flecs::Phase)
                           .depends_on(Phases.PostUpdate);
    Phases.OnRender = world.entity("Phase OnRender")
                          .add(flecs::Phase)
                          .depends_on(Phases.PreRender);
    Phases.OnDebugRender = world.entity("Phase OnDebugRender")
                               .add(flecs::Phase)
                               .depends_on(Phases.OnRender);
    Phases.PostRender = world.entity("Phase PostRender")
                            .add(flecs::Phase)
                            .depends_on(Phases.OnDebugRender);

    // Register modules
    world.import <Core>();
    world.import <Chunk>();
    world.import <Physics>();
    world.import <Renderer2DModule>();
    world.import <Tilemaps>();
    world.import <SpriteRendering>();
}
} // namespace Mana
