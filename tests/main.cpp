#include "main.h"

namespace xx::Lua {
	std::string tmpStr;
}


/*******************************************************************************************************************************/
// xx::Shared<xx::GLTexture>
/*******************************************************************************************************************************/

namespace xx::Lua::SharedGLTexture {

	inline void AppendTo(std::string& s, xx::Shared<xx::GLTexture> const& o, size_t const& ident1 = 0, size_t const& ident2 = 0) {
		Append(s, CharRepeater{ ' ', ident1 }, "xx::Shared<xx::GLTexture> {");
		if (o) {
			AppendFormat(s, R"(
{0}id   = {1}
{0}size = {2}, {3}
{0}fn   = {4})", CharRepeater{ ' ', ident2 + 4 }, *o->Get(), o->Width(), o->Height(), o->FileName());
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

	inline int __eq(lua_State* L) {
		if (lua_type(L, 2) != LUA_TUSERDATA) return Push(L, false);
		// todo: check mt equals?
		return Push(L, *(void**)lua_touserdata(L, 1) == *(void**)lua_touserdata(L, 2));
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

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},
		{"__eq", __eq},
		{"Width", Width},
		{"Height", Height},
		{"Size", Size},
		{"FileName", FileName},
		// ...
		{nullptr, nullptr}
	};
}


namespace xx::Lua {

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<std::is_same_v<xx::Shared<xx::GLTexture>, std::decay_t<T>>>> {
		using U = xx::Shared<xx::GLTexture>;
		inline static std::string name = TypeName<U>();
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, ::xx::Lua::SharedGLTexture::funcs, 0);
		}
	};

	// push instance( move or copy )
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_same_v<xx::Shared<xx::GLTexture>, std::decay_t<T>>>> {
		static constexpr int checkStackSize = 1;
		static int Push_(lua_State* const& L, T&& in) {
			return PushUserdata<xx::Shared<xx::GLTexture>>(L, std::forward<T>(in));
		}
	};

	// to pointer( for visit )
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>>&& std::is_same_v<xx::Shared<xx::GLTexture>, std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		static void To_(lua_State* const& L, int const& idx, T& out) {
			AssertType<xx::Shared<xx::GLTexture>>(L, idx);
			out = (T)lua_touserdata(L, idx);
		}
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
    pos = )", o.pos.x, ", ", o.pos.y);
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

	inline int GetTexture(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		return Push(L, o.tex);
	}

	inline int SetPosition(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.SetPosition({ To<float>(L, 2), To<float>(L, 3) });
		lua_settop(L, 1);
		return 1;
	}

	inline int GetPosition(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		return Push(L, o.pos.x, o.pos.y);
	}

	// ...

	inline int Draw(lua_State* L) {
		auto& o = *To<xx::Quad*>(L);
		o.Draw();
		return 0;
	}

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},
		{"SetTexture", SetTexture},
		{"GetTexture", GetTexture},
		{"SetPosition", SetPosition},
		{"GetPosition", GetPosition},
		// ...
		{"Draw", Draw},
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
		xx::Lua::Quad::Register(L);
		// ...

		SetGlobalCClosure(L, "xxSearchPathAdd", [](auto L)->int {
			auto dir = To<std::string_view>(L);
			xx::engine.SearchPathAdd(dir);
			return 0;
			});

		SetGlobalCClosure(L, "xxLoadSharedTexture", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			return Push(L, xx::engine.LoadSharedTexture(fn));
			});

		// ...
	}

}



/*******************************************************************************************************************************/
// GameLooper
/*******************************************************************************************************************************/

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

	xx::Lua::Engine::Register(L);

	xL::DoString(L, R"-#-(

local t = xxLoadSharedTexture("res/sword.png")
print(t)

local q = xxQuad()
q:SetTexture( t ):SetPosition( 50, 50 )
print(q)

print(t == q:GetTexture())

function Update()
	q:Draw()
end

	)-#-");
}

int GameLooper::Update() {
	xL::CallGlobalFunc(L, "Update");
	fpsViewer.Update();
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("tests");
}
