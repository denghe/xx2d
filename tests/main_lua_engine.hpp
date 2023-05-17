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
