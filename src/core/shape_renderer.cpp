#include "shape_renderer.h"

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace MGE {

static uint32_t compileShader(uint32_t type, const char* src) {
    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error(std::string("ShapeRenderer shader compile error: ") + log);
    }
    return id;
}

static uint32_t linkProgram(uint32_t vert, uint32_t frag) {
    uint32_t prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    int ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        glDeleteProgram(prog);
        throw std::runtime_error(std::string("ShapeRenderer shader link error: ") + log);
    }
    return prog;
}

ShapeRenderer::~ShapeRenderer() {
    if (m_circleVAO) { glDeleteVertexArrays(1, &m_circleVAO); m_circleVAO = 0; }
    if (m_circleVBO) { glDeleteBuffers(1,      &m_circleVBO); m_circleVBO = 0; }
    if (m_shader)    { glDeleteProgram(m_shader);              m_shader    = 0; }
}

void ShapeRenderer::init(int width, int height) {
    m_width  = width;
    m_height = height;
    buildCircleMesh();
    buildShaders();
}

void ShapeRenderer::resize(int width, int height) {
    m_width  = width;
    m_height = height;
    
}

void ShapeRenderer::drawCircle(const Transform& t, const Color& fill) {
    glUseProgram(m_shader);

    
    float proj[16] = {
        2.0f / (float)m_width,  0,                       0, 0,
        0,                      2.0f / (float)m_height,  0, 0,
        0,                      0,                       1, 0,
        0,                      0,                       0, 1,
    };

    
    const float rad = t.rotation * (float)(M_PI / 180.0);
    const float c = cosf(rad), s = sinf(rad);
    const float sx = t.scaleX,  sy = t.scaleY;
    const float ax = t.anchorX, ay = t.anchorY;
    const float tx = t.x - ax * c * sx + ay * s * sy;
    const float ty = t.y - ax * s * sx - ay * c * sy;

    float model[16] = {
        c * sx, s * sx, 0, 0,
       -s * sy, c * sy, 0, 0,
        0,      0,      1, 0,
        tx,     ty,     0, 1,
    };

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjection"), 1, GL_FALSE, proj);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "uModel"),      1, GL_FALSE, model);
    glUniform4f(glGetUniformLocation(m_shader, "uColor"), fill.r, fill.g, fill.b, fill.a);

    glBindVertexArray(m_circleVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_circleVertCount);
    glBindVertexArray(0);

    glUseProgram(0);
}

void ShapeRenderer::buildCircleMesh() {
    const int N = 64;
    std::vector<float> verts;
    verts.reserve(N * 6);

    for (int i = 0; i < N; ++i) {
        float a0 = (float)i       / N * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / N * 2.0f * (float)M_PI;
        verts.push_back(0.0f);      verts.push_back(0.0f);
        verts.push_back(cosf(a0)); verts.push_back(sinf(a0));
        verts.push_back(cosf(a1)); verts.push_back(sinf(a1));
    }
    m_circleVertCount = N * 3;

    glGenVertexArrays(1, &m_circleVAO);
    glGenBuffers(1,      &m_circleVBO);
    glBindVertexArray(m_circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShapeRenderer::buildShaders() {
    const char* vertSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 uModel;
        uniform mat4 uProjection;
        void main() {
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragSrc = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 uColor;
        void main() {
            FragColor = uColor;
        }
    )";

    uint32_t vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    uint32_t frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    m_shader = linkProgram(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

} 
