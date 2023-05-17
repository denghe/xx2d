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
			{"Width", xx::Lua::SharedGLTexture::Width},
			{"Height", xx::Lua::SharedGLTexture::Height},
			{"Size", xx::Lua::SharedGLTexture::Size},
			{"FileName", xx::Lua::SharedGLTexture::FileName},
			// ... SetGLTexParm ?
			{nullptr, nullptr}
		};
	};

}
