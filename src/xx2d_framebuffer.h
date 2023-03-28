#pragma once
#include "xx2d.h"

namespace xx {
	struct FrameBuffer {

		GLFrameBuffer fb;
		XY bak{};


		explicit FrameBuffer(bool autoInit = false) {
			if (autoInit) {
				Init();
			}
		}

		FrameBuffer& Init();

		Shared<GLTexture> MakeTexture(Pos<uint32_t> const& wh, bool const& hasAlpha = true);

		template<typename Func>
		void DrawTo(Shared<GLTexture>& t, std::optional<RGBA8> const& c, Func&& func) {
			Begin(t, c);
			func();
			End();
		}

		template<typename Func>
		Shared<GLTexture> Draw(Pos<uint32_t> const& wh, bool const& hasAlpha, std::optional<RGBA8> const& c, Func&& func) {
			auto t = MakeTexture(wh, hasAlpha);
			DrawTo(t, c, std::forward<Func>(func));
			return t;
		}

	protected:
		void Begin(Shared<GLTexture>& t, std::optional<RGBA8> const& c = {});
		void End();
	};
}
