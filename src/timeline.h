#pragma once

namespace MGE {

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

private:
    double        m_currentTime;
    double        m_startTime;
    double        m_endTime;
    double        m_fps;
    PlaybackState m_playbackState;
};

} // namespace MGE