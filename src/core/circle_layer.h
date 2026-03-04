#pragma once

#include "shape_layer.h"
#include "renderer.h"

namespace MGE {

    class CircleLayer : public ShapeLayer {
    public:
        CircleLayer(const std::string& name, Time startTime, Time duration)
            : ShapeLayer(name, startTime, duration) {
                m_transform.scaleX = 50.0f; // Default radius
                m_transform.scaleY = 50.0f; // Default radius
            }

        void renderFrame(Time currentTime, Renderer& renderer) override {
            if (!m_visible) return;
            if (currentTime < m_startTime || currentTime > m_startTime + m_duration) return;

            renderer.drawCircle(m_transform, m_fillColor);
        }
    };

}

