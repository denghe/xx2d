#pragma once
#include "pch.h"

namespace xx {

    void ZstdDecompress(std::string_view const& src, Data& dst);

}
