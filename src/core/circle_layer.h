#pragma once

#include "shape_layer.h"

namespace MGE {

    class CircleLayer : public ShapeLayer {
    public:
        CircleLayer(const std::string& name, Time startTime, Time duration)
            : ShapeLayer(name, startTime, duration) {
                m_transform.scaleX = 50.0f; // Default radius
                m_transform.scaleY = 50.0f; // Default radius
            }

        void renderFrame(Time currentTime) override {
            if (!m_visible) return;
            if (currentTime < m_startTime || currentTime > m_startTime + m_duration) return;

            // TODO: draw unit circle at (m_transform.x, m_transform.y)
            //       scaled by (m_transform.scaleX, m_transform.scaleY),
            //       rotated by m_transform.rotation, filled with m_fillColor
        }
    };

}
