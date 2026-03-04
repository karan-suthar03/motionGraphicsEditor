#pragma once

#include "color.h"
#include "transform.h"
#include "shape_renderer.h"
#include <cstdint>

namespace MGE {

    class Renderer {
    public:
        Renderer() = default;
        ~Renderer();

        void init(int width, int height);
        void resize(int width, int height);

        void beginFrame(const Color& background);
        void endFrame();

        uint32_t getTexture() const { return m_colorTex; }
        int      getWidth()   const { return m_width;    }
        int      getHeight()  const { return m_height;   }

        ShapeRenderer& shapes() { return m_shapes; }

        void drawCircle(const Transform& t, const Color& fill) { m_shapes.drawCircle(t, fill); }

    private:
        void buildFBO();
        void destroyFBO();

        uint32_t m_fbo      = 0;
        uint32_t m_colorTex = 0;
        int      m_width    = 1920;
        int      m_height   = 1080;

        ShapeRenderer m_shapes;
    };

} // namespace MGE

