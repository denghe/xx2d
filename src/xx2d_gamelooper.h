#include"xx2d.h"

namespace xx {

	struct GameLooperBase {
#ifdef __EMSCRIPTEN__
		void* wnd{};
#endif
		virtual void Init() {}						// call after EngineInit
		virtual void AfterGLInit() {}				// call after EngineGLInit
		virtual int Update() { return 0; }			// return non-zero mean quit loop
		virtual void Deinit() {}					// call before EngineDestroy
		virtual ~GameLooperBase() {};
		int Run(std::string const& wndTitle = "xx2d game engine");	// begin loop
	};

}
