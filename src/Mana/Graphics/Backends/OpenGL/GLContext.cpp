#include "GLContext.h"
#include "Mana/Core/Core.h"
#include "Mana/Graphics/Backends/OpenGL/GL.h"
#include "Mana/Graphics/Backends/OpenGL/GLAPI.h"

namespace Mana {
GLContext::GLContext(Window &window) {
    m_Window = &window;

    MC_INFO("Creating GL Context from window");
    gladLoadGL(window.CreateGLContext());

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Get Version
    const GLubyte *version = glGetString(GL_VERSION);
    MC_INFO("OpenGL Version: {}", (const char *)version);

    // Check for extensions
    GLint numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    MC_INFO("OpenGL has {} extensions", numExtensions);

    /*
    for (GLint i = 0; i < numExtensions; i++) {
        const char *ext = (const char *)glGetStringi(GL_EXTENSIONS, i);
        M_TRACE(" - {}", ext);
    }*/

    // Get some constants
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_GLInfo.MaxFragmentSamplers);
    MC_TRACE("GLMaxFragmentSamplers = {}", m_GLInfo.MaxFragmentSamplers);
    MC_TRACE(
        "MANAB_OPENGL_MAX_TEXTURE = " MACRO_TOSTRING(MANAB_OPENGL_MAX_TEXTURE));
    M_ASSERT(m_GLInfo.MaxFragmentSamplers >= MANAB_OPENGL_MAX_TEXTURE);
}

void GLContext::SwapWindowDrawBuffers() { m_Window->GLSwapBuffers(); }

GLContext::~GLContext() {}
Scope<GraphicsAPI> GLContext::GetAPI() {
    return CreateScope<GLAPI>(
        std::bind(&GLContext::SwapWindowDrawBuffers, this), m_GLInfo);
}
} // namespace Mana
