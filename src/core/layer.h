#pragma once

#include <string>
#include "time.h"

namespace MGE {

    class Layer {
    public:
        Layer(const std::string& name, Time startTime, Time duration)
            : m_name(name), m_visible(true), m_startTime(startTime), m_duration(duration) {}

        virtual ~Layer() = default;

        const std::string& getName()      const { return m_name; }
        bool               isVisible()    const { return m_visible; }
        Time               getStartTime() const { return m_startTime; }
        Time               getDuration()  const { return m_duration; }

        void setName(const std::string& name) { m_name = name; }
        void setVisible(bool visible)          { m_visible = visible; }
        void setStartTime(Time t)             { m_startTime = t; }
        void setDuration(Time d)              { m_duration = d; }

    protected:
        std::string m_name;
        bool        m_visible;
        Time        m_startTime;
        Time        m_duration;
    };

}
