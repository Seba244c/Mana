#include "Camera.h"

#include "Mana/Core/Application.h"
#include "Mana/Events/Event.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/IO/Input.h"
#include "Mana/Rendering/Cameras/Orthographic.h"
#include "Mana/World/ECS.h"

#include <flecs/addons/cpp/c_types.hpp>

namespace Mana {
void Camera::RecalculateViewMatrix() {
    const auto transform =
        translate(glm::mat4(1.0f), m_Position) *
        rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(1, 1, 1));

    m_ViewMatrix = inverse(transform); // For cameras
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

flecs::entity CameraModule::s_ActiveCamera;

CameraModule::CameraModule(flecs::world &world) {
    world.module<CameraModule>();

    // Register Components
    using Components = ECSReflect::ComponentRegistrator<
        TypesList<CameraController, OrthoCamera, MouseDirTracker>>;
    Components::RegisterAll(world);

    world
        .system<OrthoCamera, WorldPosition>(
            "Update OrthoCamera position transform")
        .kind(ECS::Phases.PreRender)
        .each([](flecs::entity e, OrthoCamera &cam, WorldPosition &pos) {
            const auto &origin = cam.GetCamera()->GetPosition();
            constexpr const float threshold = 0.0001f;

            if (glm::length(pos - origin) < threshold)
                return;

            const float d = e.world().delta_time() * 1000;
            const float t =
                cam.Lerpiness == 0.0f ? 1.0f : d / (cam.Lerpiness + d);

            cam.GetCamera()->SetPosition(glm::mix(origin, pos, t));
        });

    world
        .system<OrthoCamera, MouseDirTracker>(
            "Track mouse direction from camera")
        .kind(ECS::Phases.PreUpdate)
        .each([](flecs::entity e, OrthoCamera &cam, MouseDirTracker &mdt) {
            const glm::vec2 mouse = Input::GetMousePosition();
            const glm::ivec2 screen =
                Application::Instance()
                    ->GetWindow()
                    .GetWindowSize(); // Using the window size, so we have the
                                      // same coordinate space that the mouse
                                      // coordinate is given in
            glm::vec2 mouseScreen = {(2 * mouse.x) / screen.x - 1.0f,
                                     1.0f - (2 * mouse.y) / screen.y};

            //
            glm::vec4 farPoint =
                glm::vec4(mouseScreen.x, mouseScreen.y, 1.0f, 1.0f);
            glm::mat4 invViewProj =
                glm::inverse(cam.GetCamera()->GetProjectionMatrix() *
                             cam.GetCamera()->GetViewMatrix());
            glm::vec4 farWorld = invViewProj * farPoint;
            farWorld /= farWorld.w;
            glm::vec2 v = glm::vec3(farWorld) - cam.GetCamera()->GetPosition();
            mdt.MouseDirection = glm::normalize(v);
        });

    world.observer<OrthoCamera>("OrthoUpdateProjMatrix")
        .event(flecs::OnSet)
        .in()
        .each([](flecs::entity e, OrthoCamera &cam) { cam.Resized(); });

    EventBridge::ObserveEvent<OrthoCamera>(
        world, EventType::WindowFramebufferResize, [](void *ptr, Event *e) {
            WindowResizeEvent *event = (WindowResizeEvent *)e;
            OrthoCamera *cam = (OrthoCamera *)ptr;

            cam->AspectRatio =
                ((float)event->GetWidth()) / ((float)event->GetHeight());
            cam->Resized();
        });
}
} // namespace Mana
