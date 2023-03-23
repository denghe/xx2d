#pragma once
#include "xx2d.h"

namespace xx {

	struct Engine {
		/**********************************************************************************/
		// file system

		std::vector<std::string> searchPaths;
		std::filesystem::path tmpPath;

		// add relative base dir to searchPaths
		void SearchPathAdd(std::string_view dir);

		// search paths revert to default
		void SearchPathReset();

		// search file by searchPaths + fn. not found return ""
		std::string GetFullPath(std::string_view fn, bool fnIsFileName = true);

		// read all data by full path
		xx::Data ReadAllBytesWithFullPath(std::string_view const& fp, bool autoDecompress = true);

		// read all data by GetFullPath( fn )
		std::pair<xx::Data, std::string> ReadAllBytes(std::string_view const& fn, bool autoDecompress = true);

		/**********************************************************************************/
		// fonts

		BMFont LoadBMFont(std::string_view const& fn);

		// ...


		/**********************************************************************************/
		// texture & cache

		// key: full path
		std::unordered_map<std::string, xx::Shared<GLTexture>, xx::StringHasher<>, std::equal_to<void>> textureCache;

		// load texture from file
		GLTexture LoadTexture(std::string_view const& fn);
		xx::Shared<GLTexture> LoadSharedTexture(std::string_view const& fn);

		// load + insert or get texture from texture cache
		xx::Shared<GLTexture> LoadTextureFromCache(std::string_view const& fn);

		// unload texture from texture cache by full path
		void UnloadTextureFromCache(std::string_view const& fn);

		// unload texture from texture cache by texture
		void UnloadTextureFromCache(xx::Shared<GLTexture> const& t);

		// delete from textureCache where sharedCount == 1. return affected rows
		size_t RemoveUnusedFromTextureCache();


		/**********************************************************************************/
		// TP & frame cache( texture does not insert into textureCache )

		// key: frame key in plist( texture packer export .plist )
		std::unordered_map<std::string, xx::Shared<Frame>, xx::StringHasher<>, std::equal_to<void>> frameCache;

		// load texture packer's plist content from file & return
		TP LoadTPData(std::string_view const& fn);

		// load plist frames into cache. duplicate key name will throw exception. auto get texture from cache if exists.
		void LoadFramesFromCache(TP const& tpd);

		// unload plist frames from cache
		void UnloadFramesFromCache(TP const& tpd);

		// delete from cache where sharedCount == 1. return removed frame count
		size_t RemoveUnusedFromFrameCache();


		/**********************************************************************************/
		// window & input

		float w = 1800, h = 1000, hw = w / 2, hh = h / 2;
		std::array<XY, 10> ninePoints;
		void SetWH(float w, float h);	// will fill w, h, hw, hh, ninePoints

		XY mousePosition{ (float)w / 2, (float)h / 2 };
		std::vector<char32_t> kbdInputs;		// for store keyboard typed chars
		std::array<uint8_t, 512> kbdStates{};	// release: 0   pressed: 1   repeat: 2
		std::array<uint8_t, 32> mbtnStates{};	// mouse button states
		bool Pressed(Mbtns const& b);	// return mbtnStates[(size_t)b];
		bool Pressed(KbdKeys const& k);	// return kbdStates[(size_t)k];

		/**********************************************************************************/
		// shader

		ShaderManager sm;

		/**********************************************************************************/
		// delay funcs

		std::vector<std::function<void()>> delayFuncs;	// call + clear at Engine.UpdateEnd

		// add delay execute func to queue.
		template<typename F>
		void DelayExecute(F&& f) {
			delayFuncs.emplace_back(std::forward<F>(f));
		}

		/**********************************************************************************/
		// game loop

		double nowSecs{}, delta{};	// seconds. update before call Update()

		void Init();
		void GLInit(void* wnd);
		void UpdateBegin();
		void GLClear(RGBA8 const& c = {});	// glViewport + glClear
		void UpdateEnd();
		void Destroy();

		// ...

		/**********************************************************************************/
		// imgui

		std::function<void()> imguiInit{ [] {} }, imguiDeinit{ [] {} }, imguiUpdate{};
	};

	// default / global instance
	extern Engine engine;
}
