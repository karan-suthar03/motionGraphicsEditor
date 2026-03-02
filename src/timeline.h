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

    
    double        GetCurrentTime()   const { return m_currentTime; }
    double        GetStartTime()     const { return m_startTime; }
    double        GetEndTime()       const { return m_endTime; }
    double        GetFPS()           const { return m_fps; }
    PlaybackState GetPlaybackState() const { return m_playbackState; }

    
    double GetDuration()         const { return m_endTime - m_startTime; }
    int    GetTotalFrames()      const;   
    int    GetCurrentFrame()     const;   

    
    bool IsPlaying()   const { return m_playbackState == PlaybackState::Playing;   }
    bool IsPaused()    const { return m_playbackState == PlaybackState::Paused;    }
    bool IsScrubbing() const { return m_playbackState == PlaybackState::Scrubbing; }

    
    
    void SetCurrentTime(double t);

    
    void SetStartTime(double t);
    void SetEndTime(double t);

    
    void SetFPS(double fps);

    
    void Play();

    
    void Pause();

    
    void Scrub(double t);

    
    void StopScrubbing();

private:
    double        m_currentTime;
    double        m_startTime;
    double        m_endTime;
    double        m_fps;
    PlaybackState m_playbackState;
};

} 
