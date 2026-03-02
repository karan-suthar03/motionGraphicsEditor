#pragma once

#include <vector>
#include <string>

namespace MGE {

    struct Layer {
        std::string name;
        bool visible;
        double startTime;
        double duration;

        Layer(const std::string& n, double start, double duration)
            : name(n), visible(true), startTime(start), duration(duration) {
        }
    };

enum class PlaybackState {
    Paused,
    Playing,
    Scrubbing
};

class Timeline {
public:
    
    Timeline();

    
    double        getCurrentTime()   const { return m_currentTime; }
    double        getStartTime()     const { return m_startTime; }
    double        getEndTime()       const { return m_endTime; }
    double        getFPS()           const { return m_fps; }
    PlaybackState getPlaybackState() const { return m_playbackState; }

    
    double getDuration()         const { return m_endTime - m_startTime; }
    int    getTotalFrames()      const;
    int    getCurrentFrame()     const;


    bool isPlaying()   const { return m_playbackState == PlaybackState::Playing;   }
    bool isPaused()    const { return m_playbackState == PlaybackState::Paused;    }
    bool isScrubbing() const { return m_playbackState == PlaybackState::Scrubbing; }
    
    void setCurrentTime(double t);
    
    void setStartTime(double t);
    void setEndTime(double t);
    
    void setFPS(double fps);
    
    void play();
    
    void pause();
    
    void scrub(double t);
    
    void stopScrubbing();

    void addLayer(const Layer& layer) {
        m_layers.push_back(layer);
    }

    const std::vector<Layer>& getLayers() const {
        return m_layers;
    }

private:
    double        m_currentTime;
    double        m_startTime;
    double        m_endTime;
    double        m_fps;
	std::vector<Layer> m_layers;
    PlaybackState m_playbackState;
};

} // namespace MGE