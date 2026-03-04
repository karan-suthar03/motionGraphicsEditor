#pragma once

#include "layer.h"
#include "visual_layer.h"
#include "audio_layer.h"
#include <vector>
#include <memory>

namespace MGE {
    class Scene {
    public:
        void addLayer(std::unique_ptr<Layer> layer) {
            if (auto* vl = dynamic_cast<VisualLayer*>(layer.get()))
                m_visualLayers.push_back(vl);
            else if (auto* al = dynamic_cast<AudioLayer*>(layer.get()))
                m_audioLayers.push_back(al);

            m_layers.push_back(std::move(layer));
        }

        const std::vector<std::unique_ptr<Layer>>& getLayers()       const { return m_layers; }
        const std::vector<VisualLayer*>&           getVisualLayers() const { return m_visualLayers; }
        const std::vector<AudioLayer*>&            getAudioLayers()  const { return m_audioLayers; }

        void renderFrame(Time currentTime, Renderer& renderer) {
            for (VisualLayer* vl : m_visualLayers)
                vl->renderFrame(currentTime, renderer);
        }

    private:
        std::vector<std::unique_ptr<Layer>> m_layers;

        std::vector<VisualLayer*> m_visualLayers;
        std::vector<AudioLayer*>  m_audioLayers;
    };

}
