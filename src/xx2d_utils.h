#pragma once
#include "xx2d_pch.h"

namespace xx {

    void ZstdDecompress(std::string_view const& src, Data& dst);

}
