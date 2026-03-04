#pragma once

#include "color.h"
#include "transform.h"
#include <cstdint>

namespace MGE {

    class ShapeRenderer {
    public:
        ShapeRenderer() = default;
        ~ShapeRenderer();

        void init(int width, int height);
        void resize(int width, int height);

        void drawCircle(const Transform& t, const Color& fill);

    private:
        void buildCircleMesh();
        void buildShaders();

        uint32_t m_circleVAO       = 0;
        uint32_t m_circleVBO       = 0;
        int      m_circleVertCount = 0;
        uint32_t m_shader          = 0;

        int m_width  = 1920;
        int m_height = 1080;
    };

} // namespace MGE
