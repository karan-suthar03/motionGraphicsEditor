#pragma once

#include "layer.h"
#include "transform.h"

namespace MGE {
    class Renderer;

    class VisualLayer : public Layer {
    public:
        VisualLayer(const std::string& name, Time startTime, Time duration)
            : Layer(name, startTime, duration) {}

        virtual ~VisualLayer() = default;

        const Transform& getTransform() const  { return m_transform; }
        Transform&       getTransform()         { return m_transform; }
        void             setTransform(const Transform& t) { m_transform = t; }

        virtual void renderFrame(Time currentTime, Renderer& renderer) = 0;

    protected:
        Transform m_transform;
    };

}
