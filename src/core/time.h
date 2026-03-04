#pragma once

namespace MGE {

struct Time {
    double seconds;

    int ToFrame(double fps) const {
        return (int)(seconds * fps);
    }

    bool operator< (const Time& o) const { return seconds <  o.seconds; }
    bool operator<=(const Time& o) const { return seconds <= o.seconds; }
    bool operator> (const Time& o) const { return seconds >  o.seconds; }
    bool operator>=(const Time& o) const { return seconds >= o.seconds; }
    bool operator==(const Time& o) const { return seconds == o.seconds; }

    Time operator+(const Time& o) const { return { seconds + o.seconds }; }
    Time operator-(const Time& o) const { return { seconds - o.seconds }; }
};

} // namespace MGE
