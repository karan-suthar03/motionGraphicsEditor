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




int Timeline::getTotalFrames() const {
    return static_cast<int>(std::floor(getDuration() * m_fps));
}

int Timeline::getCurrentFrame() const {
    return static_cast<int>(std::floor(m_currentTime * m_fps));
}




void Timeline::setCurrentTime(double t) {
    if (t < m_startTime) t = m_startTime;
    if (t > m_endTime)   t = m_endTime;
    m_currentTime = t;
}

void Timeline::setStartTime(double t) {
    if (t >= m_endTime)
        throw std::invalid_argument("Timeline: start time must be less than end time.");
    m_startTime = t;
    
    if (m_currentTime < m_startTime)
        m_currentTime = m_startTime;
}

void Timeline::setEndTime(double t) {
    if (t <= m_startTime)
        throw std::invalid_argument("Timeline: end time must be greater than start time.");
    m_endTime = t;
    
    if (m_currentTime > m_endTime)
        m_currentTime = m_endTime;
}

void Timeline::setFPS(double fps) {
    if (fps <= 0.0)
        throw std::invalid_argument("Timeline: FPS must be a positive value.");
    m_fps = fps;
}




void Timeline::play() {
    m_playbackState = PlaybackState::Playing;
}

void Timeline::pause() {
    m_playbackState = PlaybackState::Paused;
}

void Timeline::scrub(double t) {
    m_playbackState = PlaybackState::Scrubbing;
    setCurrentTime(t);
}

void Timeline::stopScrubbing() {
    if (m_playbackState == PlaybackState::Scrubbing)
        m_playbackState = PlaybackState::Paused;
}

} 
