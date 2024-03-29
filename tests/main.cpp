﻿#include "main.h"

using MyLuaState = decltype(GameLooper::L);
#define RefWeakTableRefId(L) MyLuaState::Extra(L)
#include <xx_lua_shared.h>

#include "main_lua_global.hpp"
#include "main_lua_shared_gltexture.hpp"
#include "main_lua_shared_frame.hpp"
#include "main_lua_quad.hpp"
#include "main_lua_bmfont.hpp"
#include "main_lua_fpsviewer.hpp"
#include "main_lua_simple_label.hpp"
//...
#include "main_lua_engine.hpp"

void GameLooper::AfterGLInit() {
	xx::Lua::MakeUserdataWeakTable(L);
	xx::Lua::RegisterBaseEnv(L);
	xx::Lua::Engine::Register(L);
	xx::Lua::AssertTop(L, 0);

	auto [data, fullpath] = xx::engine.LoadFileData("res/test3.lua");
	xx::Lua::DoBuffer(L, data, fullpath);
}

int GameLooper::Update() {
	xx::Lua::CallGlobalFunc(L, "Update", xx::engine.delta);
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("lua tests");
}

























// todo: package searcher


//lua_getglobal(_ls, LUA_LOADLIBNAME);
//if (!lua_istable(_ls, -1))
//	return;

//lua_getfield(_ls, -1, "searchers");
//if (!lua_istable(_ls, -1))
//	return;

//lua_pushvalue(_ls, -2);
//lua_pushcclosure(_ls, mysearcher, 1);
//lua_rawseti(_ls, -2, 5);
//lua_setfield(_ls, -2, "searchers");

//xx::Coro GameLooper::QuadLogic() {
//	auto& rnd = xx::engine.rnd;
//	auto& q = quads.Emplace();
//	q.SetTexture(tex).SetPosition({ (float)rnd.Next(-500, 500), (float)rnd.Next(-300, 300) });
//	auto iter = quads.Tail();
//	for (int i = 0, e = rnd.Next(1000, 2000); i < e; ++i) {
//		q.AddRotate(1.f / 60);
//		CoYield;
//	}
//	quads.Remove(iter);
//}

//#define RUN_LUA_CODE
//
//void GameLooper::Init() {
//	//fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
//	//font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
//
//#ifdef RUN_LUA_CODE
//	xx::Lua::MakeUserdataWeakTable(L);
//	xx::Lua::RegisterBaseEnv(L);
//	xx::Lua::Engine::Register(L);
//	xx::Lua::AssertTop(L, 0);
//
//	auto [data, fullpath] = xx::engine.LoadFileData("res/test3.lua");
//	xx::Lua::DoBuffer(L, data, fullpath);
//#else
//	tex = xx::engine.LoadSharedTexture("res/tree.pkm");
//	auto n = 100000;
//	quads.Reserve(n);
//	for (int j = 0; j < n; ++j) {
//		tasks.Add(QuadLogic());
//	}
//#endif
//}

//int GameLooper::Update() {
//#ifdef RUN_LUA_CODE
//	xx::Lua::CallGlobalFunc(L, "Update", xx::engine.delta);
//#else
//	//timePool += xx::engine.delta;
//	//while (timePool >= 1.f / 60) {
//	//	timePool -= 1.f / 60;
//	tasks();
//	//}
//	quads.Foreach([&](auto& o) { o.Draw(); });
//#endif
//
//	//fpsViewer.Draw(fontBase);
//	return 0;
//}
