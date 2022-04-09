#pragma once
#include "Env.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <cassert>

#define STRUCT_BASE_CODE_CORE(T) T()=default;T(T const&)=delete;T& operator=(T const&)=delete;
#define STRUCT_BASE_CODE(T) T(T&&)=default;T& operator=(T&&)=default;STRUCT_BASE_CODE_CORE(T)

struct Xxmv {
	STRUCT_BASE_CODE_CORE(Xxmv);
	int _ki_ = -1;
	int numFrames = 0;
	Xxmv(Xxmv&& o) { this->_ki_ = o._ki_; o._ki_ = -1; }
	Xxmv& operator=(Xxmv&& o) { this->_ki_ = o._ki_; o._ki_ = -1; return *this; }
	~Xxmv() { if (_ki_ != -1) { XxmvUnload(_ki_); _ki_ = -1; } }
	operator bool() const { return _ki_ != -1; }
	void Init(std::string_view fileName) {
		_ki_ = XxmvLoad(fileName.data(), fileName.size());
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

struct Monster {
	STRUCT_BASE_CODE(Monster);

	Sprite sprite;
	Sprite shadow;
	int frameIndex = -1;
	float x = 0, y = 0;

	void Init(std::shared_ptr<Xxmv> xxmv) {
		shadow.Init(xxmv);
		shadow.SetColor(0, 0, 0);
		shadow.SetOpacity(127);
		sprite.Init(xxmv);
		Update(0);
	}

	int Update(float delta) {
		++frameIndex;
		if (frameIndex == sprite.xxmv->numFrames) frameIndex = 0;
		sprite.SetXxmvFrame(frameIndex);
		sprite.SetPosition(x, y);
		shadow.SetXxmvFrame(frameIndex);
		shadow.SetPosition(x + 10, y + 10);
		return 0;
	}
};

struct Logic {
	Sprite bg;
	std::vector<std::unique_ptr<Monster>> ms;

	Logic() {
		auto v = std::make_shared<Xxmv>();
		v->Init("zhangyu");

		bg.Init(v);
		bg.SetScale(100, 100);

		auto& m = ms.emplace_back( std::make_unique<Monster>() );
		m->x = 200;
		m->y = 200;
		m->Init(v);
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
