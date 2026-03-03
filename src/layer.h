#pragma once

#include <string>

namespace MGE {

    class Layer {
    public:
        Layer(const std::string& name, double startTime, double duration)
            : m_name(name), m_visible(true), m_startTime(startTime), m_duration(duration) {}

        virtual ~Layer() = default;

        const std::string& getName()      const { return m_name; }
        bool               isVisible()    const { return m_visible; }
        double             getStartTime() const { return m_startTime; }
        double             getDuration()  const { return m_duration; }

        void setName(const std::string& name) { m_name = name; }
        void setVisible(bool visible)          { m_visible = visible; }
        void setStartTime(double t)            { m_startTime = t; }
        void setDuration(double d)             { m_duration = d; }

    protected:
        std::string m_name;
        bool        m_visible;
        double      m_startTime;
        double      m_duration;
    };

}