#include "ClearFramebufferPass.h"

namespace Mana {
void ClearFramebufferPass::Execute(FrameData *frame) {
    API->SetClearColor(m_Color);
    API->Clear();
}
} // namespace Mana
