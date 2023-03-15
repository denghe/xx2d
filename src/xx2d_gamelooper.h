#include"xx2d.h"

namespace xx {

	struct GameLooperBase {
		virtual void Init() {}				// call after EngineInit
		virtual int Update() { return 0; }	// return non-zero mean quit loop
		virtual void Deinit() {}			// call before EngineDestroy
		virtual ~GameLooperBase() {};
		int Run();							// begin loop
	};

}
