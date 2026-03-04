#pragma once

#include "layer.h"
#include <vector>
#include <memory>

namespace MGE {
    class Scene {
    public:
        void addLayer(std::unique_ptr<Layer> layer) {
            m_layers.push_back(std::move(layer));
        }

        const std::vector<std::unique_ptr<Layer>>& getLayers() const {
            return m_layers;
        }

    private:
        std::vector<std::unique_ptr<Layer>> m_layers;
    };

}
