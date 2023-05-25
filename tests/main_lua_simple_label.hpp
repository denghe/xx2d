/*******************************************************************************************************************************/
// xx::SimpleLabel
/*******************************************************************************************************************************/

namespace xx::Lua::SimpleLabel {

	inline void Register(lua_State* const& L, char const* const& keyName = "xxSimpleLabel") {
		SetGlobalCClosure(L, keyName, [](auto L)->int { return PushUserdata<xx::SimpleLabel>(L); });
	}

	inline int __tostring(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		auto& s = tmpStr;
		s.clear();

		Append(s, R"(xx::SimpleLabel {
    tex = )");
		xx::Lua::SharedGLTexture::AppendTo(s, o.tex, 0, 4);

		Append(s, R"(
    pos = {)", o.pos.x, ", ", o.pos.y, "}");

		Append(s, R"(
    anchor = {)", o.anchor.x, ", ", o.anchor.y, "}");

		Append(s, R"(
    scale = {)", o.scale.x, ", ", o.scale.y, "}");

		Append(s, R"(
    baseScale = )", o.baseScale);

		Append(s, R"(
    color = {)", o.color.r, ", ", o.color.g, ", ", o.color.b, ", ", o.color.a, "}");

		Append(s, R"(
    size = {)", o.size.x, ", ", o.size.y, "}");

		Append(s, R"(
})");

		return Push(L, s);
	}

	inline int SetText(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		auto& bmf = *To<xx::BMFont*>(L, 2);
		auto text = To<std::string_view>(L, 3);
		auto n = lua_gettop(L);
		if (n == 3) {
			o.SetText(bmf, text);
		} else if (n == 4) {
			o.SetText(bmf, text, To<float>(L, 4));
		} else {
			o.SetText(bmf, text, To<float>(L, 4), To<float>(L, 5));
		}
		lua_settop(L, 1);
		return 1;
	}

	inline int SetAnchor(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetAnchor(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetScale(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		if (lua_gettop(L) == 2) {
			o.SetScale(To<float>(L, 2));
		} else {
			o.SetScale(To<xx::XY>(L, 2));
		}
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPosition(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetPosition(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPositionX(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetPositionX(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetPositionY(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetPositionY(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPosition(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.AddPosition(To<xx::XY>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPositionX(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.AddPositionX(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int AddPositionY(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.AddPositionY(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColor(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetColor(To<RGBA8>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColorA(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetColorA(To<uint8_t>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int SetColorAf(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.SetColorAf(To<float>(L, 2));
		lua_settop(L, 1);
		return 1;
	}

	inline int Draw(lua_State* L) {
		auto& o = *To<xx::SimpleLabel*>(L);
		o.Draw();
		return 0;
	}


	// get QuadInstanceData
	inline int GetTexture(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->tex); }
	inline int GetPosition(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->pos); }
	inline int GetAnchor(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->anchor); }
	inline int GetScale(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->scale); }
	inline int GetBaseScale(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->baseScale); }
	inline int GetColor(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->color); }
	inline int GetSize(lua_State* L) { return Push(L, To<xx::SimpleLabel*>(L)->size); }


	inline luaL_Reg funcs[] = {
		{"__tostring", __tostring},

		{"SetText", SetText},
		{"SetAnchor", SetAnchor},
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
		{"GetBaseScale", GetBaseScale},
		{"GetColor", GetColor},
		{"GetSize", GetSize},

		{nullptr, nullptr}
	};
}


namespace xx::Lua {

	// meta
	template<typename T>
	struct MetaFuncs<T, std::enable_if_t<std::is_same_v<xx::SimpleLabel, std::decay_t<T>>>> {
		using U = xx::SimpleLabel;
		inline static std::string name = TypeName<U>();
		static void Fill(lua_State* const& L) {
			SetType<U>(L);
			luaL_setfuncs(L, ::xx::Lua::SimpleLabel::funcs, 0);
		}
	};

	// to T*
	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> && std::is_same_v<xx::SimpleLabel, std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>>> {
		static void To_(lua_State* const& L, int const& idx, T& out) {
			AssertType<xx::SimpleLabel>(L, idx);
			out = (T)lua_touserdata(L, idx);
		}
	};

}
