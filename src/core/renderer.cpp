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
    m_shapes.init(width, height);
}

void Renderer::resize(int width, int height) {
    m_width  = width;
    m_height = height;
    buildFBO();
    m_shapes.resize(width, height);
}

void Renderer::beginFrame(const Color& bg) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame() {
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
}

void Renderer::destroyFBO() {
    if (m_fbo)      { glDeleteFramebuffers(1, &m_fbo);     m_fbo      = 0; }
    if (m_colorTex) { glDeleteTextures(1,    &m_colorTex); m_colorTex = 0; }
}

} // namespace MGE
