/*******************************************************************************************************************************/
// xx::engine
/*******************************************************************************************************************************/

namespace xx::Lua::Engine {

	inline void Register(lua_State* const& L) {
		xx::Lua::Data::Register(L);
		xx::Lua::Quad::Register(L);
		xx::Lua::BMFont::Register(L);
		xx::Lua::FpsViewer::Register(L);
		// ...

		/**********************************************************************************/
		// file system

		// set lua package.path by xx::engine.searchPaths
		SetGlobalCClosure(L, "xxSearchPathSync", [](auto L)->int {
			xx_assert(xx::engine.searchPaths.size());
			tmpStr.clear();
			for (size_t i = 0, e = xx::engine.searchPaths.size(); i < e; ++i) {
				tmpStr.append(xx::engine.searchPaths[i]);
				tmpStr.append("?.lua;");
			}
			tmpStr.pop_back();	// remove last ';'
			xx::Lua::CheckStack(L, 3);
			lua_getglobal(L, "package");											// ..., t
			SetField(L, "path", tmpStr);
			lua_pop(L, 1);															// ...
			return 0;
			});

		SetGlobalCClosure(L, "xxSearchPathAdd", [](auto L)->int {
			auto dir = To<std::string_view>(L);
			auto insertToFront = lua_gettop(L) > 1 ? To<bool>(L, 2) : false;
			xx::engine.SearchPathAdd(dir, insertToFront);
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

		SetGlobalCClosure(L, "xxLoadFileData", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			if (lua_gettop(L) > 1) {
				auto autoDecompress = To<bool>(L, 2);
				return Push(L, xx::engine.LoadFileData(fn, autoDecompress));
			} else {
				return Push(L, xx::engine.LoadFileData(fn));
			}
			return 0;
			});

		SetGlobalCClosure(L, "xxDetectFileFormat", [](auto L)->int {
			auto d = To<xx::Data*>(L);
			return Push(L, xx::engine.DetectFileFormat(*d));
			});

		/**********************************************************************************/
		// fonts


		SetGlobalCClosure(L, "xxLoadBMFont", [](auto L)->int {
			auto fn = To<std::string_view>(L);
			return Push(L, xx::engine.LoadBMFont(fn));
			});

		/**********************************************************************************/
		// texture & cache

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
