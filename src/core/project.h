#pragma once

#include "timeline.h"
#include "scene.h"
#include "color.h"
#include <string>

namespace MGE {

struct Resolution {
    int width  = 1920;
    int height = 1080;
};

enum class ColorSpace {
    sRGB,
    LinearRGB,
    ACES
};


struct ProjectMetadata {
    Resolution  resolution       = { 1920, 1080 };
    ColorSpace  colorSpace       = ColorSpace::sRGB;
    float       unitScale        = 1.0f;
    Color       backgroundColor  = { 0.0f, 0.0f, 0.0f, 1.0f };
};

class Project {
public:
    
    
    Project();

    
    explicit Project(const std::string& name);

    
    Project(const Project&)            = delete;
    Project& operator=(const Project&) = delete;

    
    Project(Project&&)            = default;
    Project& operator=(Project&&) = default;

    ~Project() = default;

    
    const std::string& GetID()      const { return m_id; }
    const std::string& GetName()    const { return m_name; }
    int                GetVersion() const { return m_version; }

    void SetName(const std::string& name);

    
    void BumpVersion();

    
    const ProjectMetadata& GetMetadata() const { return m_metadata; }
    void                   SetMetadata(const ProjectMetadata& meta);

    
    void SetResolution(int width, int height);
    void SetColorSpace(ColorSpace cs);
    void SetUnitScale(float scale);
    void SetBackgroundColor(const Color& color);

    
    Timeline&       GetTimeline()       { return m_timeline; }
    const Timeline& GetTimeline() const { return m_timeline; }

    Scene&          GetScene()          { return m_scene; }
    const Scene&    GetScene()    const { return m_scene; }

    int  GetSelectedLayerIndex() const            { return m_selectedLayerIndex; }
    void SetSelectedLayerIndex(int idx)           { m_selectedLayerIndex = idx; }

private:
    std::string     m_id;
    std::string     m_name;
    int             m_version;
    ProjectMetadata m_metadata;
    Timeline        m_timeline;
    Scene           m_scene;
    int             m_selectedLayerIndex = -1;

    
    static std::string GenerateID();
};

} 
