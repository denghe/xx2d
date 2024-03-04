/*******************************************************************************************************************************/
// xx::FpsViewer
/*******************************************************************************************************************************/

namespace xx::Lua::FpsViewer {

	inline void Register(lua_State* const& L, char const* const& keyName = "xxFpsViewer") {
		SetGlobalCClosure(L, keyName, [](auto L)->int { return PushUserdata<::xx::FpsViewer>(L); });
	}

	inline int __tostring(lua_State* L) {
		auto& o = *To<::xx::FpsViewer*>(L);
		return Push(L, "xx::FpsViewer {}");
	}

	inline int Draw(lua_State* L) {
		auto& o = *To<::xx::FpsViewer*>(L);
		auto& fnt = *To<::xx::BMFont*>(L, 2);
		auto n = lua_gettop(L);
		if (n == 2) {
			o.Draw(fnt);
		} else if (n == 3) {
			o.Draw(fnt, To<float>(L, 3));
		} else if (n > 4) {
			o.Draw(fnt, To<float>(L, 3), ::xx::XY{ To<float>(L, 4), To<float>(L, 5) });
		}
		return 0;
	}

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},

		{"Draw", Draw},

		{nullptr, nullptr}
	};
}


namespace xx::Lua {

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<std::is_same_v<xx::FpsViewer, std::decay_t<T>>>> {
		using U = xx::FpsViewer;
		inline static std::string name{ TypeName<U>() };
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, ::xx::Lua::FpsViewer::funcs, 0);
		}
	};

	// to T*
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> && std::is_same_v<xx::FpsViewer, std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		static void To_(lua_State* const& L, int const& idx, T& out) {
			AssertType<xx::FpsViewer>(L, idx);
			out = (T)lua_touserdata(L, idx);
		}
	};

}
