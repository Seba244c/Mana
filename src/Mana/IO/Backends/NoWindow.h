#pragma once
#include "mpch.h"

#include "Mana/IO/Window.h"

namespace Mana {
class NoWindow final : public Window {
  public:
    // Window State Set
    void PollEvents() override {}
    void Show() override {}
    void Fullscreen(bool fullscreen) override {}
    void SetTitle(std::string &title) override {}
    void SetPosition(int x, int y) override {}
    void CenterWindow() override {}
    glm::ivec2 GetSize() override { return {0, 0}; }
    void SetEventCallback(const EventCallbackFn &callback) override {}

    // Window State Get
    [[nodiscard]] bool ShouldWindowClose() const override { return false; }
    [[nodiscard]] bool IsFullscreen() const override { return false; }
    [[nodiscard]] virtual void *GetNativeWindow() const override {
        return nullptr;
    };
};
} // namespace Mana
