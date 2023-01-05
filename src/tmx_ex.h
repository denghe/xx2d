#pragma once
#include "pch.h"

namespace TMX {
	/*
┌─────────────────┐                                    ┌──────────────────┐
│   screen        │                                    │     screen       │
│                 │                                    │                  │
│        ┌────────┼────────────────────────────────────┼─────────┐        │
│        │0,0     │                                    │         │        │
│        │        │                 map                │         │        │
└────────┼────────┘                                    └─────────┼────────┘
         │                  ┌──────────────────┐                 │
         │                  │  screen          │                 │
         │                  │                  │                 │
         │                  │         *        │                 │
         │                  │        pos       │                 │
         │                  │                  │                 │
         │                  └──────────────────┘                 │
┌────────┼─────────┐                                   ┌─────────┼────────┐
│        │         │                                   │         │        │
│        │         │                                   │         │        │
│        └─────────┼───────────────────────────────────┼─────────┘        │
│    screen        │                                   │     screen       │
│                  │                                   │                  │
└──────────────────┘                                   └──────────────────┘
	*/
	struct Camera : Translate {
		// fill by Init
		int32_t tileWidth = 0, tileHeight = 0;
		int32_t worldRowCount = 0, worldColumnCount = 0;
		Size worldPixel{};
		Size screenSize{};
		XY pos{};
		bool dirty = true;

		/*
			for (uint32_t y = cam.rowFrom; y < cam.rowTo; ++y) {
				for (uint32_t x = cam.columnFrom; x < cam.columnTo; ++x) {
					auto&& s = ss[y * cam.worldColumnCount + x];
		*/
		int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

		void Init(Size const& screenSize, Map& map);

		void SetScreenSize(Size const& wh);
		void SetScale(float const& scale);
		void SetPos(XY const& xy);

		// call after set xxxx ...
		void Commit();

		void Draw(Engine* eg, std::vector<Sprite>& ss);
	};

	void Fill(std::vector<Sprite>& ss, Map const& map, Layer_Tile const& lt);

}
