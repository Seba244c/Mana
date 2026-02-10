#pragma once
#include "mpch.h"

#include "Mana/Graphics/GraphicsAPI.h"

namespace Mana {
class GraphicsContext {
  public:
    virtual ~GraphicsContext() = default;
    virtual Scope<GraphicsAPI> GetAPI() = 0;
};
} // namespace Mana
