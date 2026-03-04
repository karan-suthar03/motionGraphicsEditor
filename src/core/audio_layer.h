#pragma once

#include "layer.h"

namespace MGE {

    class AudioLayer : public Layer {
    public:
        AudioLayer(const std::string& name, Time startTime, Time duration)
            : Layer(name, startTime, duration) {}

        virtual ~AudioLayer() = default;
    };

}
