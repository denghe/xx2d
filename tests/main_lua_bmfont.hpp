/*******************************************************************************************************************************/
// xx::BMFont
/*******************************************************************************************************************************/

namespace xx::Lua::BMFont {

	inline int __tostring(lua_State* L) {
		auto& o = *To<xx::BMFont*>(L);
		auto& s = tmpStr;
		s.clear();

		Append(s, R"(xx::BMFont {
    fullPath = )", o.fullPath);

		Append(s, R"(
    fontSize = )", o.fontSize);

		Append(s, R"(
    lineHeight = )", o.lineHeight);

		Append(s, R"(
})");

		return Push(L, s);
	}

	inline int Load(lua_State* L) {
		auto& o = *To<xx::BMFont*>(L);
		auto fn = To<std::string_view>(L, 2);
		o.Load(fn);
		return 0;
	}

	inline int GetFullPath(lua_State* L) { return Push(L, To<xx::BMFont*>(L)->fullPath); }
	inline int GetFontSize(lua_State* L) { return Push(L, To<xx::BMFont*>(L)->fontSize); }
	inline int GetLineHeight(lua_State* L) { return Push(L, To<xx::BMFont*>(L)->lineHeight); }

	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},

		{"Load", Load},
		{"GetFullPath", GetFullPath},
		{"GetFontSize", GetFontSize},
		{"GetLineHeight", GetLineHeight},

		{nullptr, nullptr}
	};
}


namespace xx::Lua {

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<std::is_same_v<xx::BMFont, std::decay_t<T>>>> {
		using U = xx::BMFont;
		inline static std::string name = TypeName<U>();
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, ::xx::Lua::BMFont::funcs, 0);
		}
	};

	// to T*
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> && std::is_same_v<xx::BMFont, std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		static void To_(lua_State* const& L, int const& idx, T& out) {
			AssertType<xx::BMFont>(L, idx);
			out = (T)lua_touserdata(L, idx);
		}
	};

}
