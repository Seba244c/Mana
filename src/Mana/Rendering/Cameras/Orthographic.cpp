#include "Mana/Rendering/Camera.h"

#include "Orthographic.h"

namespace Mana {
OrthographicCamera::OrthographicCamera(const float left, const float right,
                                       const float bottom, const float top)
    : Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f), 1.0f) {}

void OrthographicCamera::SetProjection(const float left, const float right,
                                       const float bottom, const float top) {
    MC_TRACE("SetProjection, left: {}, right: {}, bottom: {}, top:{}", left,
             right, bottom, top);
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

OrthoCamera::OrthoCamera(const float aspectRatio, const float zoomLevel,
                         const float lerpiness)
    : m_Camera({-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel,
                zoomLevel}),
      AspectRatio(aspectRatio), ZoomLevel(zoomLevel), Lerpiness(lerpiness) {}

void OrthoCamera::Resized() {
    MC_TRACE("OrthoCamera::Resized, AspectRatio: {}, ZoomLevel: {}",
             AspectRatio, ZoomLevel);

    m_Camera.SetProjection(-AspectRatio * ZoomLevel, AspectRatio * ZoomLevel,
                           -ZoomLevel, ZoomLevel);
}
} // namespace Mana
