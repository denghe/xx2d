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
		{"GetRotate", GetRadians},
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
