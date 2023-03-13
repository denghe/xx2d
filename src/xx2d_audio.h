#pragma once
#include "xx2d.h"

namespace xx {

	struct Audio {
		void* ctx{}, * bg{};

		double cd{};
		std::vector<std::pair<std::string, double>> fncds;

		// todo: file cache? maybe need it?

		Audio();
		~Audio();
		void PlayBG(std::string_view const& fn);
		void StopBG();
		void Play(std::string_view const& fn);
		bool CanPlay(std::string_view const& fn);
	};

}
