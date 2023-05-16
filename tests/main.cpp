#include "main.h"

namespace xx::Lua {
	std::string tmpStr;
	int luaWeakTableRefId{};

	inline void MakeUserdataWeakTable(lua_State* L) {
		// create weak table for userdata cache
		xx::Lua::AssertTop(L, 0);
		lua_createtable(L, 0, 1000);											// ... t
		lua_createtable(L, 0, 1);												// ... t, mt
		xx::Lua::SetField(L, "__mode", "v");									// ... t, mt { __mode = v }
		lua_setmetatable(L, -2);												// ... t
		luaWeakTableRefId = luaL_ref(L, LUA_REGISTRYINDEX);						// ...
		xx::Lua::AssertTop(L, 0);
	}

	// wt push to stack
	inline void GetUserdataWeakTable(lua_State* L) {
		assert(luaWeakTableRefId);
		CheckStack(L, 4);
		lua_rawgeti(L, LUA_REGISTRYINDEX, luaWeakTableRefId);					// ..., wt
	}

	// store. value at top stack
	inline void StoreUserdataToWeakTable(lua_State* L, void* key) {
		GetUserdataWeakTable(L);												// ..., v, wt
		lua_pushvalue(L, -2);													// ..., v, wt, v
		lua_rawsetp(L, -2, key);												// ..., v, wt
		lua_pop(L, 1);															// ..., v
	}

	// found: push to stack & return true
	inline bool TryGetUserdataFromWeakTable(lua_State* L, void* key) {
		GetUserdataWeakTable(L);												// ..., wt
		auto typeId = lua_rawgetp(L, -1, key);									// ..., wt, v?
		if (typeId == LUA_TNIL) {
			lua_pop(L, 2);														// ...
			return false;
		}
		lua_replace(L, -2);														// ..., v
		return true;
	}

	inline void RemoveUserdataFromWeakTable(lua_State* L, void* key) {
		GetUserdataWeakTable(L);												// ..., wt
		lua_pushnil(L);															// ..., wt, nil
		lua_rawsetp(L, -2, key);												// ..., wt
		lua_pop(L, 1);															// ...
	}

	// match for Shared<T> luaL_setfuncs
	template<typename T, typename ENABLED = void>
	struct SharedTFuncs {
		inline static luaL_Reg funcs[] = {
			// ...
			{nullptr, nullptr}
		};
	};

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<xx::IsShared_v<T>>> {
		using U = std::decay_t<T>;
		inline static std::string name = TypeName<U>();
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, SharedTFuncs<U>::funcs, 0);
			xx::Lua::SetFieldCClosure(L, "__gc", [](lua_State* L)->int {
				auto& p = *To<U*>(L);
				RemoveUserdataFromWeakTable(L, p.pointer);
				p.~U();
				return 0;
			});
		}
	};

	// push instance( move or copy )
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<xx::IsShared_v<T>>> {
		using U = std::decay_t<T>;
		static constexpr int checkStackSize = 1;
		static int Push_(lua_State* const& L, T&& in) {
			if (!in) {
				lua_pushnil(L);
			} else if (!TryGetUserdataFromWeakTable(L, in.pointer)) {
				auto bak = in.pointer;
				PushUserdata<U, true>(L, std::forward<T>(in));
				StoreUserdataToWeakTable(L, bak);
			}
			return 1;
		}
	};

	// to pointer( for visit )
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> && xx::IsShared_v<std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		using U = std::decay_t<std::remove_pointer_t<std::decay_t<T>>>;
		static void To_(lua_State* const& L, int const& idx, T& out) {
			if (lua_isnil(L, idx)) {
				out = nullptr;
			} else {
				AssertType<U>(L, idx);
				out = (T)lua_touserdata(L, idx);
			}
		}
	};
}


/*******************************************************************************************************************************/
// type mappings
/*******************************************************************************************************************************/

namespace xx::Lua {

	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_same_v<xx::XY, std::decay_t<T>>>> {
		static constexpr int checkStackSize = 2;
		static int Push_(lua_State* const& L, T&& in) {
			return Push(L, in.x, in.y);
		}
		static void To_(lua_State* const& L, int const& idx, T& out) {
			To(L, idx, out.x, out.y);
		}
	};

	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_same_v<xx::RGBA8, std::decay_t<T>>>> {
		static constexpr int checkStackSize = 4;
		static int Push_(lua_State* const& L, T&& in) {
			return Push(L, in.r, in.g, in.b, in.a);
		}
		static void To_(lua_State* const& L, int const& idx, T& out) {
			To(L, idx, out.r, out.g, out.b, out.a);
		}
	};

}


/*******************************************************************************************************************************/
// xx::Shared<xx::GLTexture>
/*******************************************************************************************************************************/

namespace xx::Lua::SharedGLTexture {

	inline void AppendTo(std::string& s, xx::Shared<xx::GLTexture> const& o, size_t const& ident1 = 0, size_t const& ident2 = 0) {
		Append(s, CharRepeater{ ' ', ident1 }, "xx::Shared<xx::GLTexture> {");
		if (o) {
			CharRepeater cr{ ' ', ident2 + 4 };

			Append(s, R"(
)", cr, "id = ", *o->Get());

			Append(s, R"(
)", cr, "size = {", o->Width(), ", ", o->Height(), "}");

			Append(s, R"(
)", cr, "fn = ", o->FileName());

			Append(s, R"(
)", CharRepeater{ ' ', ident2 }, "}");

		} else {
			Append(s, "}");
		}
	}

	inline int __tostring(lua_State* L) {
		auto& o = *To<xx::Shared<xx::GLTexture>*>(L);
		auto& s = tmpStr;
		s.clear();
		AppendTo(s, o);
		return Push(L, s);
	}

	inline int Width(lua_State* L) {
		auto& o = *To<xx::Shared<xx::GLTexture>*>(L);
		if (!o) return 0;
		return Push(L, o->Width());
	}

	inline int Height(lua_State* L) {
		auto& o = *To<xx::Shared<xx::GLTexture>*>(L);
		if (!o) return 0;
		return Push(L, o->Height());
	}

	inline int Size(lua_State* L) {
		auto& o = *To<xx::Shared<xx::GLTexture>*>(L);
		if (!o) return 0;
		return Push(L, o->Width(), o->Height());
	}

	inline int FileName(lua_State* L) {
		auto& o = *To<xx::Shared<xx::GLTexture>*>(L);
		if (!o) return 0;
		return Push(L, o->FileName());
	}
}

namespace xx::Lua {

	template<typename T>
	struct SharedTFuncs<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, xx::Shared<xx::GLTexture>>>> {
		inline static luaL_Reg funcs[] = {
			{"__tostring", xx::Lua::SharedGLTexture::__tostring},
			//{"__eq", xx::Lua::SharedGLTexture::__eq},
			{"Width", xx::Lua::SharedGLTexture::Width},
			{"Height", xx::Lua::SharedGLTexture::Height},
			{"Size", xx::Lua::SharedGLTexture::Size},
			{"FileName", xx::Lua::SharedGLTexture::FileName},
			// ... SetGLTexParm ?
			{nullptr, nullptr}
		};
	};

}


/*******************************************************************************************************************************/
// xx::Shared<xx::Frame>
/*******************************************************************************************************************************/

namespace xx::Lua::SharedFrame {

	inline void AppendTo(std::string& s, xx::Shared<xx::Frame> const& o, size_t const& ident1 = 0, size_t const& ident2 = 0) {
		Append(s, CharRepeater{ ' ', ident1 }, "xx::Shared<xx::Frame> {");
		if (o) {
			CharRepeater cr{ ' ', ident2 + 4 };

			Append(s, R"(
)", cr, "tex = ");
			xx::Lua::SharedGLTexture::AppendTo(s, o->tex, ident2 + 4);

			Append(s, R"(
)", cr, "key = ", o->key);

			Append(s, R"(
)", cr, "anchor = ");
			if (o->anchor.has_value()) {
				Append(s, "{", o->anchor->x, ", ", o->anchor->y, "}");
			} else {
				Append(s, "{}");
			}

			Append(s, R"(
)", cr, "spriteOffset = {", o->spriteOffset.x, ", ", o->spriteOffset.y, "}");

			Append(s, R"(
)", cr, "spriteSize = {", o->spriteSize.x, ", ", o->spriteSize.y, "}");

			Append(s, R"(
)", cr, "spriteSourceSize = {", o->spriteSourceSize.x, ", ", o->spriteSourceSize.y, "}");

			Append(s, R"(
)", cr, "textureRect = {", o->textureRect.x, ", ", o->textureRect.y, ", ", o->textureRect.wh.x, ", ", o->textureRect.wh.y, "}");

			Append(s, R"(
)", cr, "ud = ", o->ud);

			Append(s, R"(
)", cr, "textureRotated = ", o->textureRotated);

			Append(s, R"(
)", cr, "triangles = ", o->triangles);

			Append(s, R"(
)", cr, "vertices = ", o->vertices);

			Append(s, R"(
)", cr, "verticesUV = ", o->verticesUV);

			Append(s, R"(
)", CharRepeater{ ' ', ident2 }, "}");

		} else {
			Append(s, "}");
		}
	}

	inline int __tostring(lua_State* L) {
		auto& o = *To<xx::Shared<xx::Frame>*>(L);
		auto& s = tmpStr;
		s.clear();
		AppendTo(s, o);
		return Push(L, s);
	}

}


namespace xx::Lua {

	template<typename T>
	struct SharedTFuncs<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, xx::Shared<xx::Frame>>>> {
		inline static luaL_Reg funcs[] = {
			{"__tostring", xx::Lua::SharedFrame::__tostring},
			//{"__eq", xx::Lua::SharedFrame::__eq},
			// ... SetGLTexParm ?
			{nullptr, nullptr}
		};
	};

}




/*******************************************************************************************************************************/
// xx::Quad
/*******************************************************************************************************************************/

namespace xx::Lua::Quad {

	inline void Register(lua_State* const& L, char const* const& keyName = "xxQuad") {
		SetGlobalCClosure(L, keyName, [](auto L)->int { return PushUserdata<xx::Quad>(L); });
	}

	inline int __tostring(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		auto& s = tmpStr;
		s.clear();

		Append(s, R"(xx::Quad {
    tex = )");
		xx::Lua::SharedGLTexture::AppendTo(s, o.tex, 0, 4);

		Append(s, R"(
    pos = {)", o.pos.x, ", ", o.pos.y, "}");

		Append(s, R"(
    anchor = {)", o.anchor.x, ", ", o.anchor.y, "}");

		Append(s, R"(
    scale = {)", o.scale.x, ", ", o.scale.y, "}");

		Append(s, R"(
    radians = )", o.radians);

		Append(s, R"(
    color = {)", o.color.r, ", ", o.color.g, ", ", o.color.b, ", ", o.color.a, "}");

		Append(s, R"(
    texRect = {)", o.texRectX, ", ", o.texRectY, ", ", o.texRectW, ", ", o.texRectH, "}");

		Append(s, R"(
})");

		return Push(L, s);
	}

	inline int SetTexture(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		auto& t = *To<xx::Shared<xx::GLTexture>*>(L, 2);
		o.SetTexture(t);
		lua_settop(L, 1);
		return 1;
	}

	inline int SetFrame(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		auto& f = *To<xx::Shared<xx::Frame>*>(L, 2);
		o.SetFrame(f);
		lua_settop(L, 1);
		return 1;
	}

	inline int Size(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		return Push(L, o.Size());
	}

	inline int SetAnchor(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetAnchor(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetRotate(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetRotate(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddRotate(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.AddRotate(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetScale(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		if (lua_gettop(L) == 2) {
			o.SetScale(To<float>(L, 2));
		} else {
			o.SetScale(To<xx::XY>(L, 2));
		}
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPosition(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetPosition(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPositionX(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetPositionX(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPositionY(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetPositionY(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPosition(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.AddPosition(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPositionX(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.AddPositionX(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPositionY(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.AddPositionY(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColor(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetColor(To<RGBA8>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColorA(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetColorA(To<uint8_t>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColorAf(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetColorAf(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int Draw(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.Draw();
		return 0;
	}


	// get QuadInstanceData
	inline int GetTexture(lua_State* L) { return Push(L, To<xx::Quad*>(L)->tex); }
	inline int GetPosition(lua_State* L) { return Push(L, To<xx::Quad*>(L)->pos); }
	inline int GetAnchor(lua_State* L) { return Push(L, To<xx::Quad*>(L)->anchor); }
	inline int GetScale(lua_State* L) { return Push(L, To<xx::Quad*>(L)->scale); }
	inline int GetRadians(lua_State* L) { return Push(L, To<xx::Quad*>(L)->radians); }
	inline int GetColor(lua_State* L) { return Push(L, To<xx::Quad*>(L)->color); }
	inline int GetTexRect(lua_State* L) { 
		auto& o = *To<xx::Quad*>(L);
		return Push(L, o.texRectX, o.texRectY, o.texRectW, o.texRectH);
	}


	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},

		{"SetTexture", SetTexture},
		{"SetFrame", SetFrame},
		{"Size", Size},
		{"SetAnchor", SetAnchor},
		{"SetRotate", SetRotate},
		{"AddRotate", AddRotate},
		{"SetScale", SetScale},
		{"SetPosition", SetPosition},
		{"SetPositionX", SetPositionX},
		{"SetPositionY", SetPositionY},
		{"AddPosition", AddPosition},
		{"AddPositionX", AddPositionX},
		{"AddPositionY", AddPositionY},
		{"SetColor", SetColor},
		{"SetColorA", SetColorA},
		{"SetColorAf", SetColorAf},
		{"Draw", Draw},

		{"GetTexture", GetTexture},
		{"GetPosition", GetPosition},
		{"GetAnchor", GetAnchor},
		{"GetScale", GetScale},
		{"GetRadians", GetRadians},
		{"GetColor", GetColor},
		{"GetTexRect", GetTexRect},

		{nullptr, nullptr}
	};
}


namespace xx::Lua {

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<std::is_same_v<xx::Quad, std::decay_t<T>>>> {
		using U = xx::Quad;
		inline static std::string name = TypeName<U>();
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, ::xx::Lua::Quad::funcs, 0);
		}
	};

	// to T*
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> && std::is_same_v<xx::Quad, std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		static void To_(lua_State* const& L, int const& idx, T& out) {
			AssertType<xx::Quad>(L, idx);
			out = (T)lua_touserdata(L, idx);
		}
	};

}




/*******************************************************************************************************************************/
// xx::engine
/*******************************************************************************************************************************/

namespace xx::Lua::Engine {

	inline void Register(lua_State* const& L) {
		xx::Lua::Data::Register(L);
		xx::Lua::Quad::Register(L);
		xx::Lua::MakeUserdataWeakTable(L);
		// ...

		SetGlobalCClosure(L, "xxSearchPathAdd", [](auto L)->int {
			auto dir = To<std::string_view>(L);
			xx::engine.SearchPathAdd(dir);
			return 0;
			});

		SetGlobalCClosure(L, "xxSearchPathReset", [](auto L)->int {
			xx::engine.SearchPathReset();
			return 0;
			});

		SetGlobalCClosure(L, "xxGetFullPath", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			if (lua_gettop(L) > 1) {
				auto fnIsFileName = To<bool>(L, 2);
				return Push(L, xx::engine.GetFullPath(fn, fnIsFileName));
			} else {
				return Push(L, xx::engine.GetFullPath(fn));
			}
			return 0;
			});

		SetGlobalCClosure(L, "xxLoadFileDataWithFullPath", [](auto L)->int {
			auto fp = To<std::string_view>(L);
			if (lua_gettop(L) > 1) {
				auto autoDecompress = To<bool>(L, 2);
				return Push(L, xx::engine.LoadFileDataWithFullPath(fp, autoDecompress));
			} else {
				return Push(L, xx::engine.LoadFileDataWithFullPath(fp));
			}
			return 0;
			});

		SetGlobalCClosure(L, "LoadFileData", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			if (lua_gettop(L) > 1) {
				auto autoDecompress = To<bool>(L, 2);
				return Push(L, xx::engine.LoadFileData(fn, autoDecompress));
			} else {
				return Push(L, xx::engine.LoadFileData(fn));
			}
			return 0;
			});

		SetGlobalCClosure(L, "DetectFileFormat", [](auto L)->int {
			auto d = To<xx::Data*>(L);
			return Push(L, xx::engine.DetectFileFormat(*d));
			});

		// ...
		//BMFont LoadBMFont(std::string_view const& fn);

		// ...

		SetGlobalCClosure(L, "xxLoadSharedTexture", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			return Push(L, xx::engine.LoadSharedTexture(fn));
			});

		// ...

		SetGlobalCClosure(L, "xxRndNextFloat", [](auto L)->int {
			switch (lua_gettop(L)) {
			case 0:
				return Push(L, xx::engine.rnd.Next<float>());
			case 1:
				return Push(L, xx::engine.rnd.Next<float>(0, To<float>(L, 1)));
			case 2:
				return Push(L, xx::engine.rnd.Next<float>(To<float>(L, 1), To<float>(L, 2)));
			default:
				return 0;
			}
			});

		SetGlobalCClosure(L, "xxRndNext", [](auto L)->int {
			switch (lua_gettop(L)) {
			case 0:
				return Push(L, xx::engine.rnd.Next());
			case 1:
				return Push(L, xx::engine.rnd.Next(0, To<int32_t>(L, 1)));
			case 2:
				return Push(L, xx::engine.rnd.Next(To<int32_t>(L, 1), To<int32_t>(L, 2)));
			default:
				return 0;
			}
			});

		// ...
	}

}



/*******************************************************************************************************************************/
// GameLooper
/*******************************************************************************************************************************/

xx::Coro GameLooper::QuadLogic() {
	auto& rnd = xx::engine.rnd;
	auto& q = quads.Emplace();
	q.SetTexture(tex).SetPosition({ (float)rnd.Next(-500, 500), (float)rnd.Next(-300, 300) });
	auto iter = quads.Tail();
	for (int i = 0, e = rnd.Next(1000, 2000); i < e; ++i) {
		q.AddRotate(1.f / 60);
		CoYield;
	}
	quads.Remove(iter);
}

#define RUN_LUA_CODE

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

#ifdef RUN_LUA_CODE
	MakeUserdataWeakTable(L);
	xx::Lua::RegisterBaseEnv(L);
	xx::Lua::Engine::Register(L);
	xx::Lua::AssertTop(L, 0);

	auto [data, fullpath] = xx::engine.LoadFileData("res/test3.lua");
	xx::Lua::DoBuffer(L, data, fullpath);
#else
	tex = xx::engine.LoadSharedTexture("res/tree.pkm");
	auto n = 100000;
	quads.Reserve(n);
	for (int j = 0; j < n; ++j) {
		coros.Add(QuadLogic());
	}
#endif
}

int GameLooper::Update() {
#ifdef RUN_LUA_CODE
	//xx::Lua::CallGlobalFunc(L, "Update", xx::engine.delta);
#else
	//timePool += xx::engine.delta;
	//while (timePool >= 1.f / 60) {
	//	timePool -= 1.f / 60;
		coros();
	//}
	quads.Foreach([&](auto& o) { o.Draw(); });
#endif

	fpsViewer.Update();
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("tests");
}


//xx::Coros cs;
//for (size_t i = 0; i < 1000000; i++) {
//	cs.Add([]()->xx::Coro {
//		while (true) {
//			CoYield;
//		}
//	}());
//}
//auto secs = xx::NowEpochSeconds();
//for (size_t i = 0; i < 100; i++) {
//	cs();
//}
//xx::CoutN(xx::NowEpochSeconds(secs));

//return 0;



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




// todo: use lua weak table cache sptr ud. key = sptr.value, value = ud
// push: find lua weak table。nullptr push nil/ return 0


