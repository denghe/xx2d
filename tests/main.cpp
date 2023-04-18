#include "main.h"
#include <xx2d_spine.h>

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

}

int GameLooper::Update() {
	coros();

	fpsViewer.Update();
	return 0;
}

int main() {
	xx::List<int> ii;
	ii.Add(1);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);
	ii.Add(ii[0]);


	return 0;

	//auto g = std::make_unique<GameLooper>();
	//return g->Run("xx2d's examples");
}
