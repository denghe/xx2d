#pragma once
#include "xx2d.h"
#include "xx2d_fps_viewer.h"

struct Monster;
struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;

	static constexpr float fps = 60.f, fds = 1.f / fps;
	xx::List<xx::Shared<Monster>> monsters;
	double timePool{};

	void Init() override;
	int Update() override;
};

struct Monster {
	GameLooper& owner;
	Monster(GameLooper& owner) : owner(owner), coro(Logic()) {}
	xx::Coro coro;
	xx::Coro Logic() {
		while (true) {
			for (size_t i = 0; i < 60; i++) {		// idle 1 seconds
				CoYield;
				std::cout << ".";
			}
			std::cout << "!" << std::endl;
		}
	}
};
