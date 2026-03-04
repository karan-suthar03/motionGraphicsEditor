#pragma once

#include "time.h"

namespace MGE {

enum class PlaybackState {
    Paused,
    Playing,
    Scrubbing
};

class Timeline {
public:
    
    Timeline();

    
    Time          getCurrentTime()   const { return m_currentTime; }
    Time          getStartTime()     const { return m_startTime; }
    Time          getEndTime()       const { return m_endTime; }
    double        getFPS()           const { return m_fps; }
    PlaybackState getPlaybackState() const { return m_playbackState; }

    
    Time   getDuration()         const { return Time{m_endTime.seconds - m_startTime.seconds}; }
    int    getTotalFrames()      const;
    int    getCurrentFrame()     const;


    bool isPlaying()   const { return m_playbackState == PlaybackState::Playing;   }
    bool isPaused()    const { return m_playbackState == PlaybackState::Paused;    }
    bool isScrubbing() const { return m_playbackState == PlaybackState::Scrubbing; }
    
    void setCurrentTime(Time t);
    
    void setStartTime(Time t);
    void setEndTime(Time t);
    
    void setFPS(double fps);
    
    void play();
    
    void pause();
    
    void scrub(Time t);
    
    void stopScrubbing();

private:
    Time          m_currentTime;
    Time          m_startTime;
    Time          m_endTime;
    double        m_fps;
    PlaybackState m_playbackState;
};

} // namespace MGE
