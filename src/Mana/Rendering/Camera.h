#pragma once
#include "mpch.h"

#include "Mana/World/Component.h"

namespace Mana {
struct CameraModule {
    CameraModule(flecs::world &world);

    struct ActiveCamera {};

  private:
    static flecs::entity s_ActiveCamera;
};

struct Camera {
  public:
    Camera(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix)
        : m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix),
          m_ViewProjectionMatrix(projectionMatrix * viewMatrix) {}
    ~Camera() = default;

    void RecalculateViewMatrix();

    [[nodiscard]] const glm::mat4 &GetProjectionMatrix() const {
        return m_ProjectionMatrix;
    }
    [[nodiscard]] const glm::mat4 &GetViewMatrix() const {
        return m_ViewMatrix;
    }
    [[nodiscard]] const glm::mat4 &GetViewProjectionMatrix() const {
        return m_ViewProjectionMatrix;
    }

    // For modifying ViewMatrix
    [[nodiscard]] const glm::vec3 &GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3 &position) {
        m_Position = position;
        RecalculateViewMatrix();
    }

    [[nodiscard]] const glm::vec3 &GetRotation() const { return m_Rotation; }
    void SetRotation(const glm::vec3 &rotation) {
        m_Rotation = rotation;
        RecalculateViewMatrix();
    }

  protected:
    glm::vec3 m_Position = glm::vec3(0.0f);

    // Rotation around the z axis
    glm::vec3 m_Rotation = glm::vec3{0.0f};

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;
};

struct CameraController {
    COMPONENT_NAME(CameraController);
    COMPONENT_ENSURE(CameraController);

    static void Describe(flecs::component<CameraController> c) {
        c.add(flecs::Inheritable);
    }

    virtual ~CameraController() = default;
    virtual Camera *GetCamera() = 0;
};

struct MouseDirTracker {
    COMPONENT_NAME(MouseDirectionTracker);

    glm::vec2 MouseDirection;
};
} // namespace Mana
