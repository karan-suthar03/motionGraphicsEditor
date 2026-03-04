#include "renderer.h"

#include <glad/glad.h>

namespace MGE {

Renderer::~Renderer() {
    destroyFBO();
}

void Renderer::init(int width, int height) {
    m_width  = width;
    m_height = height;
    buildFBO();
    m_projWidth  = width;
    m_projHeight = height;
    m_shapes.init(width, height);
}

void Renderer::resize(int width, int height) {
    m_width  = width;
    m_height = height;
    buildFBO();
}

void Renderer::setProjectionSize(int projW, int projH) {
    if (m_projWidth == projW && m_projHeight == projH)
        return;
    m_projWidth  = projW;
    m_projHeight = projH;
    m_shapes.setProjectionSize(projW, projH);
}

void Renderer::beginFrame(const Color& bg) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    glViewport(0, 0, m_width, m_height);
    glEnable(GL_MULTISAMPLE);
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glBlitFramebuffer(0, 0, m_width, m_height,
                      0, 0, m_width, m_height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::buildFBO() {
    destroyFBO();

    glGenTextures(1, &m_colorTex);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenRenderbuffers(1, &m_msaaRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, k_msaaSamples, GL_RGBA8, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_msaaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_msaaRbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::destroyFBO() {
    if (m_msaaFbo) { glDeleteFramebuffers(1,  &m_msaaFbo); m_msaaFbo = 0; }
    if (m_msaaRbo) { glDeleteRenderbuffers(1, &m_msaaRbo); m_msaaRbo = 0; }
    if (m_fbo)      { glDeleteFramebuffers(1, &m_fbo);     m_fbo      = 0; }
    if (m_colorTex) { glDeleteTextures(1,    &m_colorTex); m_colorTex = 0; }
}

} // namespace MGE
