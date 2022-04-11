#pragma once
#include "Env.h"
#include "Logic.h"
#include <array>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <unordered_set>
#include <cassert>

uint64_t Now() {
	return GetNow();
}

uint64_t SteadyNow() {
	return GetSteadyNow();
}

#define STRUCT_BASE_CODE_CORE(T) T()=default;T(T const&)=delete;T& operator=(T const&)=delete;
#define STRUCT_BASE_CODE(T) T(T&&)=default;T& operator=(T&&)=default;STRUCT_BASE_CODE_CORE(T)

struct Xxmv {
	STRUCT_BASE_CODE_CORE(Xxmv);
	int _ki_ = -1;
	int numFrames = 0;
	Xxmv(Xxmv&& o) { this->_ki_ = o._ki_; o._ki_ = -1; }
	Xxmv& operator=(Xxmv&& o) { this->_ki_ = o._ki_; o._ki_ = -1; return *this; }
	~Xxmv() { if (_ki_ != -1) { XxmvDelete(_ki_); _ki_ = -1; } }
	operator bool() const { return _ki_ != -1; }
	void Init(std::string_view fileName) {
		_ki_ = XxmvNew((void*)fileName.data(), fileName.size());
		numFrames = XxmvGetFramesCount(_ki_);
	}
};

struct Sprite {
	STRUCT_BASE_CODE_CORE(Sprite);

	int _ki_ = -1;
	std::shared_ptr<Xxmv> xxmv;
	Sprite(Sprite&& o) { this->_ki_ = o._ki_; o._ki_ = -1; }
	Sprite& operator=(Sprite&& o) { this->_ki_ = o._ki_; o._ki_ = -1; return *this; }
	~Sprite() { if (_ki_ != -1) { SpriteDelete(_ki_); _ki_ = -1; } }
	operator bool() const { return _ki_ != -1; }
	void Init(std::shared_ptr<Xxmv> xxmv) { _ki_ = SpriteNew(xxmv->_ki_, -1); this->xxmv = xxmv; }
	void SetPosition(float x, float y) { SpriteSetPosition(_ki_, x, y); }
	void SetZOrder(float z) { SpriteSetZOrder(_ki_, z); }
	void SetAnchor(float x, float y) { SpriteSetAnchor(_ki_, x, y); }
	void SetRotation(float r) { SpriteSetRotation(_ki_, r); }
	void SetScale(float x, float y) { SpriteSetScale(_ki_, x, y); }
	void SetColor(uint8_t r, uint8_t g, uint8_t b) { SpriteSetColor(_ki_, r, g, b); }
	void SetOpacity(uint8_t o) { SpriteSetOpacity(_ki_, o); }
	void SetVisible(bool b) { SpriteSetVisible(_ki_, b); }
	void SetXxmvFrame(int i) { SpriteSetXxmvFrame(_ki_, xxmv->_ki_, i); }
	// todo: more
};

struct Random3 {
	STRUCT_BASE_CODE_CORE(Random3);
	uint64_t seed = 1234567891234567890;
	inline uint32_t Next() {
		seed ^= (seed << 21u);
		seed ^= (seed >> 35u);
		seed ^= (seed << 4u);
		return (uint32_t)seed;
	}
	inline int Next(int min, int max) {
		return (Next() % (max - min)) + min;
	}
};

struct Monster {
	STRUCT_BASE_CODE(Monster);

	Sprite body;
	Sprite shadow;
	int numFrames = 0;
	int frameIndex = -1;
	float x = 0, y = 0;

	void Init(std::shared_ptr<Xxmv> xxmv) {
		numFrames = xxmv->numFrames;
		shadow.Init(xxmv);
		shadow.SetColor(0, 0, 0);
		shadow.SetOpacity(127);
		body.Init(xxmv);
		Update(0);
	}

	int Update(float delta) {
		++frameIndex;
		if (frameIndex == numFrames) frameIndex = 0;
		body.SetXxmvFrame(frameIndex);
		body.SetPosition(x, y);
		shadow.SetXxmvFrame(frameIndex);
		shadow.SetPosition(x + 10, y + 10);
		return 0;
	}
};

struct Logic {
	std::array<uint8_t, 1024> inBuf, outBuf;

	Sprite bg;
	std::vector<std::shared_ptr<Monster>> ms;

	std::shared_ptr<Monster> MakeMonster(float x, float y) {
		auto& m = ms.emplace_back(std::make_shared<Monster>());
		m->x = x;
		m->y = y;
		m->Init(mv);
		return m;
	}

	Random3 rnd;
	std::shared_ptr<Xxmv> mv;

	int TouchBegan(int ki, float x, float y) {
		MakeMonster(x, y);

		// 测试发现 该 函数调用性能 大约是 luajit 的 20 倍
		//auto t = Now();
		//for (size_t i = 0; i < 100000000; i++) {
		//	bg.SetPosition(i, i);
		//}
		//auto te = (Now() - t) / 10000;
		//ConsoleLog((void*)std::to_string(te).c_str());

		return 1;
	};

	void TouchMoved(int ki, float x, float y) {
		MakeMonster(x, y);
	};

	void TouchEnded(int ki, float x, float y) {
	};

	void TouchCancelled(int ki) {
	};

	Logic() {
		mv = std::make_shared<Xxmv>();
		mv->Init("zhangyu");

		bg.Init(mv);
		bg.SetScale(100, 100);
		bg.SetColor(0, 0, 255);
	}

	void Update(float delta) {
		for (auto i = (int)ms.size() - 1; i >= 0; --i) {
			if (ms[i]->Update(delta)) {
				std::swap(ms[i], ms.back());
				ms.pop_back();
			}
		}
	}
};

extern "C" {
	void* LogicNew() {
		return new Logic();
	}

	void LogicDelete(void* logic) {
		delete (Logic*)logic;
	}

	void* LogicGetInBuf(void* logic) {
		return ((Logic*)logic)->inBuf.data();
	}

	void* LogicGetOutBuf(void* logic) {
		return ((Logic*)logic)->outBuf.data();
	}

	int LogicTouchBegan(void* logic, int ki, float x, float y) {
		return ((Logic*)logic)->TouchBegan(ki, x, y);
	};

	void LogicTouchMoved(void* logic, int ki, float x, float y) {
		((Logic*)logic)->TouchMoved(ki, x, y);
	};

	void LogicTouchEnded(void* logic, int ki, float x, float y) {
		((Logic*)logic)->TouchEnded(ki, x, y);
	};

	void LogicTouchCancelled(void* logic, int ki) {
		((Logic*)logic)->TouchCancelled(ki);
	};

	void LogicUpdate(void* logic, float delta) {
		((Logic*)logic)->Update(delta);
	}
}
