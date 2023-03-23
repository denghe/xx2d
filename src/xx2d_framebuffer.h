#pragma once
#include "xx2d.h"

namespace xx {
	struct FrameBuffer {
		GLFrameBuffer fb;
		XY bak{};

		void Init();

		Shared<GLTexture> MakeTex(Pos<> const& wh, bool const& hasAlpha = true);

		template<typename Func>
		void DrawTo(Shared<GLTexture>& t, RGBA8 const& c, Func&& func) {
			Begin(t, c);
			func();
			End();
		}

		template<typename Func>
		static Shared<GLTexture> Draw(Pos<> const& wh, bool const& hasAlpha, RGBA8 const& c, Func&& func) {
			FrameBuffer f;
			f.Init();
			auto t = f.MakeTex(wh, hasAlpha);
			f.DrawTo(t, c, std::forward<Func>(func));
			return t;
		}

	protected:
		void Begin(Shared<GLTexture>& t, RGBA8 const& c = {});
		void End();
	};
}
