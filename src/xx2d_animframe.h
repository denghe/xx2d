#pragma once
#include "xx2d_pch.h"

namespace xx {
    struct Frame {
        Shared<GLTexture> tex;
        std::string key;
        // std::vector<std::string> aliases;	// unused
        std::optional<XY> anchor;
        XY spriteOffset{};
        XY spriteSize{};		// content size
        XY spriteSourceSize{};	// original pic size
        Rect textureRect{};
        bool textureRotated{};
        std::vector<uint16_t> triangles;
        std::vector<float> vertices;
        std::vector<float> verticesUV;
    };

    struct AnimFrame {
        Shared<Frame> frame;
        float durationSeconds;
    };

    using AnimFrames = std::vector<AnimFrame>;

    struct Anim {
        AnimFrames afs;
        size_t cursor = 0;
        float timePool = 0;
        void Step();
        bool Update(float const& delta);
        AnimFrame& GetCurrentAnimFrame() const;
    };
}
