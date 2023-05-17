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

	// todo
}


namespace xx::Lua {

	template<typename T>
	struct SharedTFuncs<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, xx::Shared<xx::Frame>>>> {
		inline static luaL_Reg funcs[] = {
			{"__tostring", xx::Lua::SharedFrame::__tostring},
			// ...
			{nullptr, nullptr}
		};
	};

}
