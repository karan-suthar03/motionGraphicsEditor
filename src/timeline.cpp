#include "timeline.h"

#include <cmath>
#include <stdexcept>

namespace MGE {




Timeline::Timeline()
    : m_currentTime(0.0)
    , m_startTime(0.0)
    , m_endTime(10.0)
    , m_fps(60.0)
    , m_playbackState(PlaybackState::Paused)
{}




int Timeline::GetTotalFrames() const {
    return static_cast<int>(std::floor(GetDuration() * m_fps));
}

int Timeline::GetCurrentFrame() const {
    return static_cast<int>(std::floor(m_currentTime * m_fps));
}




void Timeline::SetCurrentTime(double t) {
    if (t < m_startTime) t = m_startTime;
    if (t > m_endTime)   t = m_endTime;
    m_currentTime = t;
}

void Timeline::SetStartTime(double t) {
    if (t >= m_endTime)
        throw std::invalid_argument("Timeline: start time must be less than end time.");
    m_startTime = t;
    
    if (m_currentTime < m_startTime)
        m_currentTime = m_startTime;
}

void Timeline::SetEndTime(double t) {
    if (t <= m_startTime)
        throw std::invalid_argument("Timeline: end time must be greater than start time.");
    m_endTime = t;
    
    if (m_currentTime > m_endTime)
        m_currentTime = m_endTime;
}

void Timeline::SetFPS(double fps) {
    if (fps <= 0.0)
        throw std::invalid_argument("Timeline: FPS must be a positive value.");
    m_fps = fps;
}




void Timeline::Play() {
    m_playbackState = PlaybackState::Playing;
}

void Timeline::Pause() {
    m_playbackState = PlaybackState::Paused;
}

void Timeline::Scrub(double t) {
    m_playbackState = PlaybackState::Scrubbing;
    SetCurrentTime(t);
}

void Timeline::StopScrubbing() {
    if (m_playbackState == PlaybackState::Scrubbing)
        m_playbackState = PlaybackState::Paused;
}

} 
