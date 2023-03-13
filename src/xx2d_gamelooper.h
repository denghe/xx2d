#include"xx2d.h"

namespace xx {

	struct GameLooperBase {
		virtual int Init() { return 0; }
		virtual int Update() { return 0; }
		virtual void Deinit() {}
		virtual ~GameLooperBase() {};
		int Run();
	};

}
