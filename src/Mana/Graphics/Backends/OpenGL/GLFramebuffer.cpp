#include "mpch.h"

#include "GLFramebuffer.h"

#include "Mana/Graphics/Backends/OpenGL/GL.h"
#include "Mana/Graphics/Framebuffer.h"
#include "Mana/Graphics/Texture.h"

namespace Mana {
static const uint32_t s_MaxFramebufferSize = 8192;

static bool IsDepthFormat(FramebufferTextureFormat format) {
    return format == FramebufferTextureFormat::DEPTH24STENCIL8 ? true : false;
}

static bool IsColorFormat(FramebufferTextureFormat format) {
    switch (format) {
    case FramebufferTextureFormat::R8:
        return true;
        break;
    case FramebufferTextureFormat::RGBA8:
        return true;
        break;
    default:
        return false;
    }
}

GLFramebuffer::GLFramebuffer(const FramebufferSpecification &specs)
    : m_Specification(specs) {
    if (specs.Width < 1 || specs.Height < 1 ||
        specs.Width > s_MaxFramebufferSize ||
        specs.Height > s_MaxFramebufferSize) {
        MC_ERROR("Attempted to reisze framebuffer to invalid size: {0}x{1}",
                 specs.Width, specs.Height);
        return;
    }

    // Sort attachments
    for (auto spec : m_Specification.Attachments.Attachments) {
        if (IsDepthFormat(spec.TextureFormat))
            m_DepthAttachmentSpecification = spec;
        else
            m_ColorAttachmentSpecifications.emplace_back(spec);
    }

    // Create fbo attachments
    Invalidate();
}

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_RendererID); }

void GLFramebuffer::Invalidate() {
    // Delete previous framebuffer
    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);

        m_ColorAttachments.clear();
        if (m_DepthAttachment)
            m_DepthAttachment = nullptr;
    }

    glCreateFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Color attachments
    if (!m_ColorAttachmentSpecifications.empty()) {
        m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

        for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++) {
            GLenum format = m_ColorAttachmentSpecifications[i].TextureFormat ==
                                    FramebufferTextureFormat::RGBA8
                                ? GL_RGBA8
                                : GL_R8;

            m_ColorAttachments[i] = CreateRef<GLTexture>(
                m_Specification.Width, m_Specification.Height, format,
                FilteringMethod::LINEAR, FilteringMethod::LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D,
                                   m_ColorAttachments[i]->GetRendererID(), 0);
        }
    }

    // Create depth
    if (m_DepthAttachmentSpecification.TextureFormat !=
        FramebufferTextureFormat::None) {
        M_ASSERT(m_DepthAttachmentSpecification.TextureFormat ==
                 FramebufferTextureFormat::DEPTH24STENCIL8);

        m_DepthAttachment = CreateRef<GLTexture>(
            m_Specification.Width, m_Specification.Height, GL_DEPTH24_STENCIL8,
            FilteringMethod::LINEAR, FilteringMethod::LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D,
                               m_DepthAttachment->GetRendererID(), 0);
    }

    if (m_ColorAttachments.size() > 1) {
        M_ASSERT(m_ColorAttachments.size() <= 4);
        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(m_ColorAttachments.size(), buffers);

    } else if (m_ColorAttachments.empty()) {
        // Only depth-pass
        glDrawBuffer(GL_NONE);
    }

    M_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
                 GL_FRAMEBUFFER_COMPLETE,
             "Framebuffer is incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void GLFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void GLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void GLFramebuffer::Resize(uint32_t width, uint32_t height) {
    if (width < 1 || height < 1 || width > s_MaxFramebufferSize ||
        height > s_MaxFramebufferSize) {
        MC_WARN("Attempted to reisze framebuffer to invalid size: {0}x{1}",
                width, height);
        return;
    }

    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate(); // To recreate
}

void GLFramebuffer::Clear() {
    glClearColor(0, 0, 0, 0);
    if (m_DepthAttachment)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT);
}
} // namespace Mana
