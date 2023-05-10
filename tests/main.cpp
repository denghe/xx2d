#include "main.h"

namespace xx::Lua {
	std::string tmpStr;
}


/*******************************************************************************************************************************/
// xx::Shared<xx::GLTexture>
/*******************************************************************************************************************************/

namespace xx::Lua::SharedGLTexture {

	inline int __tostring(lua_State* L) {
		auto q = To<xx::Shared<xx::GLTexture>*>(L);
		auto& s = tmpStr;
		s.clear();
		Append(s, "xx::Shared<xx::GLTexture>{ texid = ... xy = ... wh = ... fn = ... }");
		return Push(L, s);
	}

	// Get...Set...

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},
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
		auto q = To<xx::Quad*>(L);
		auto& s = tmpStr;
		s.clear();
		Append(s, "xx::Quad{ tex = ..., xy = ... ... }");
		return Push(L, s);
	}

	// Get...Set...

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},
		// ...
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
			auto dir = To<char const*>(L);
			xx::engine.SearchPathAdd(dir);
			return 0;
			});

		SetGlobalCClosure(L, "xxLoadSharedTexture", [](auto L)->int {
			auto fn = To<char const*>(L);
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
print(q)

function LuaUpdate(a)
	print( a )
end

	)-#-");

	coros.Add([](GameLooper* self)->xx::Coro {
		CoYield;
		for (size_t i = 0; i < 5; i++) {
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
