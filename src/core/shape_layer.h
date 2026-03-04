#pragma once

#include "visual_layer.h"

namespace MGE {

    struct Color {
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    };

    class ShapeLayer : public VisualLayer {
    public:
        ShapeLayer(const std::string& name, Time startTime, Time duration)
            : VisualLayer(name, startTime, duration) {}

        virtual ~ShapeLayer() = default;

        Color getFillColor() const    { return m_fillColor; }
        void  setFillColor(Color c)   { m_fillColor = c; }

    protected:
        Color m_fillColor;
    };

}
