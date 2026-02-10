#pragma once

#include "mpch.h"

#include "Mana/Rendering/Camera.h"
#include "Mana/World/Component.h"
#include "Mana/World/Core.h"

namespace Mana {
class OrthographicCamera : public Camera {
  public:
    OrthographicCamera(float left, float right, float bottom, float top);

    // For modifying ProjectionMatrix
    void SetProjection(float left, float right, float bottom, float top);
};

struct OrthoCamera : CameraController {
    COMPONENT_NAME(OrthographicCamera);
    COMPONENT_ENSURE(Position, Rotation);

    OrthoCamera()
        : m_Camera({1.0f, 1.0f, -1.0f, 1.0f}), AspectRatio(1.0f),
          ZoomLevel(10.0f), Lerpiness(100.0f) {}

    OrthoCamera(float aspectRatio, float zoomLevel = 10.0f,
                float lerpiness = 100);

    Camera *GetCamera() override { return &m_Camera; }
    void Resized();
    float AspectRatio;
    float ZoomLevel;
    float Lerpiness = 100;

    static void Describe(flecs::component<OrthoCamera> c) {
        c.is_a<CameraController>();
        /*        fields.push_back(
            ComponentFieldDesc(Float, "AspectRatio", sizeof(CameraController)));
        fields.push_back(ComponentFieldDesc(
            Float, "ZoomLevel", sizeof(CameraController) + sizeof(float)));
        fields.push_back(ComponentFieldDesc(
            Float, "Lerpiness", sizeof(CameraController) + 2 *
        sizeof(float)));*/
    }

  private:
    OrthographicCamera m_Camera;
};
} // namespace Mana
