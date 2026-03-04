#include "project.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>

namespace MGE {




std::string Project::GenerateID() {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dist(0, 15);
    std::uniform_int_distribution<> dist8(8, 11); 

    
    std::ostringstream ss;
    ss << std::hex;

    for (int i = 0; i < 8;  ++i) ss << dist(gen);
    ss << '-';
    for (int i = 0; i < 4;  ++i) ss << dist(gen);
    ss << '-' << '4';
    for (int i = 0; i < 3;  ++i) ss << dist(gen);
    ss << '-' << dist8(gen);
    for (int i = 0; i < 3;  ++i) ss << dist(gen);
    ss << '-';
    for (int i = 0; i < 12; ++i) ss << dist(gen);

    return ss.str();
}




Project::Project()
    : m_id(GenerateID())
    , m_name("Untitled Project")
    , m_version(1)
    , m_metadata{}
    , m_timeline{}
{}

Project::Project(const std::string& name)
    : m_id(GenerateID())
    , m_name(name)
    , m_version(1)
    , m_metadata{}
    , m_timeline{}
{}




void Project::SetName(const std::string& name) {
    if (name.empty())
        throw std::invalid_argument("Project: name must not be empty.");
    m_name = name;
}

void Project::BumpVersion() {
    ++m_version;
}




void Project::SetMetadata(const ProjectMetadata& meta) {
    if (meta.resolution.width  <= 0 || meta.resolution.height <= 0)
        throw std::invalid_argument("Project: resolution dimensions must be positive.");
    if (meta.unitScale <= 0.0f)
        throw std::invalid_argument("Project: unit scale must be positive.");
    m_metadata = meta;
}

void Project::SetResolution(int width, int height) {
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("Project: resolution dimensions must be positive.");
    m_metadata.resolution = { width, height };
}

void Project::SetColorSpace(ColorSpace cs) {
    m_metadata.colorSpace = cs;
}

void Project::SetUnitScale(float scale) {
    if (scale <= 0.0f)
        throw std::invalid_argument("Project: unit scale must be positive.");
    m_metadata.unitScale = scale;
}

void Project::SetBackgroundColor(const Color& color) {
    m_metadata.backgroundColor = color;
}

} 
