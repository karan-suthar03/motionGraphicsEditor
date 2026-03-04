#pragma once

namespace MGE {

    struct Transform {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        float rotation = 0.0f;

        float scaleX = 1.0f;
        float scaleY = 1.0f;

        float anchorX = 0.0f;
        float anchorY = 0.0f;
    };

}
