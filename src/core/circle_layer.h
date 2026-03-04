#pragma once

#include "shape_layer.h"
#include "renderer.h"

namespace MGE {

    class CircleLayer : public ShapeLayer {
    public:
        CircleLayer(const std::string& name, Time startTime, Time duration)
            : ShapeLayer(name, startTime, duration) {}

        float getRadius() const        { return m_radius; }
        void  setRadius(float radius)  { m_radius = radius; }

        void renderFrame(Time currentTime, Renderer& renderer) override {
            if (!m_visible) return;
            if (currentTime < m_startTime || currentTime > m_startTime + m_duration) return;

            Transform drawTf  = m_transform;
            drawTf.scaleX     = m_radius * m_transform.scaleX;
            drawTf.scaleY     = m_radius * m_transform.scaleY;
            renderer.drawCircle(drawTf, m_fillColor);
        }

    private:
        float m_radius = 50.0f;
    };

}

