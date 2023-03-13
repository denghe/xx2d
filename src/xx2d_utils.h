#pragma once
#include "xx2d.h"

namespace xx {

    void ZstdDecompress(std::string_view const& src, Data& dst);

}
