#include "main.h"

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

	xL::DoString(L, R"-#-(

print("hello world")

function LuaUpdate(a)
	print( a )
end

	)-#-");

	coros.Add([](GameLooper* self)->xx::Coro {
		CoYield;
		for (size_t i = 0; i < 100; i++) {
			xL::CallGlobalFunc(self->L, "LuaUpdate", i);
			CoYield;
		}
	}(this));
}

int GameLooper::Update() {
	coros();

	fpsViewer.Update();
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("tests");
}
