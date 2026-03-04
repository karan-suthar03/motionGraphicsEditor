#pragma once

namespace MGE {

struct Time {
    double seconds;

    int ToFrame(double fps) const {
        return (int)(seconds * fps);
    }
};

} // namespace MGE
