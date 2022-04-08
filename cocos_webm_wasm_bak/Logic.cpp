#pragma once
#include "Env.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cassert>

#define STRUCT_BASE_CODE(T) \
Monster() = default; \
Monster(Monster const&) = default; \
Monster(Monster&&) = default; \
Monster& operator=(Monster const&) = default; \
Monster& operator=(Monster&&) = default;

struct Monster {
	STRUCT_BASE_CODE(Monster);
	std::string xxmvName;
	int ki = -1;
	int numFrames = 0;
	int frameIndex = 0;	// 1 ~ numFrames
	float x = 0, y = 0;

	void Update(float delta) {
		++frameIndex;
		if (frameIndex == numFrames) frameIndex = 1;
		SpriteSetFrameName(ki, (char*)(xxmvName + "_" + std::to_string(frameIndex)).c_str());
		SpriteSetPosition(ki, x, y);
	}
};

struct Logic {
	std::unordered_map<int, std::unique_ptr<Monster>> ms;

	Logic() {
		int r = XxmvLoad("st_m_20");
		assert(r > 0);

		auto m = std::make_unique<Monster>();
		m->xxmvName = "st_m_20";
		m->numFrames = r;
		auto ki = SpriteNew("st_m_20_1");
		m->ki = ki;
		m->x = 200;
		m->y = 200;
		m->Update(0);

		ms.insert({ ki, std::move(m) });
	}

	~Logic() {
		// todo
	}

	void Update(float delta) {
		for (auto& m : ms) {
			m.second->Update(delta);
		}
	}
};

Logic* __logic = nullptr;
extern "C" {
	void LogicInit() {
		__logic = new Logic();
	}
	void LogicUninit() {
		delete __logic;
	}
	void LogicUpdate(float delta) {
		__logic->Update(delta);
	}
}
