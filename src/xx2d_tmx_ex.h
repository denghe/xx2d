#pragma once
#include "xx2d.h"

namespace xx {

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
		struct Camera {
			// fill by Init
			int32_t tileWidth = 0, tileHeight = 0;
			int32_t worldRowCount = 0, worldColumnCount = 0;
			XY worldPixel{};
			XY screenSize{};

			AffineTransform at;
			XY pos{}, scale{ 1, 1 };
			bool dirty = true;

			/*
				for (uint32_t y = cam.rowFrom; y < cam.rowTo; ++y) {
					for (uint32_t x = cam.columnFrom; x < cam.columnTo; ++x) {
						auto&& s = ss[y * cam.worldColumnCount + x];
			*/
			int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

			void Init(XY const& screenSize, Map& map);

			void SetScreenSize(XY const& wh);
			void SetScale(float const& scale);
			void SetPosition(XY const& xy);
			void SetPositionX(float const& x);
			void SetPositionY(float const& y);

			// call after set xxxx ...
			void Commit();
		};


		template<typename LT>
		constexpr LayerTypes GetLayerType() {
			if constexpr (std::is_same_v<LT, Layer_Tile>) {
				return LayerTypes::TileLayer;
			} else if constexpr (std::is_same_v<LT, Layer_Object>) {
				return LayerTypes::ObjectLayer;
			} else if constexpr (std::is_same_v<LT, Layer_Image>) {
				return LayerTypes::ImageLayer;
			} else if constexpr (std::is_same_v<LT, Layer_Group>) {
				return LayerTypes::GroupLayer;
			} else return LayerTypes::MAX_VALUE_UNKNOWN;
		}
		template<typename LT>
		constexpr LayerTypes LayerTypeEnum_v = GetLayerType<LT>();

		template<typename LT>
		void Fill(std::vector<LT*>& out, std::vector<xx::Shared<Layer>>& ls) {
			for (auto& l : ls) {
				if (l->type == LayerTypes::GroupLayer) {
					Fill(out, ((Layer_Group&)*l).layers);
				} else if (l->type == LayerTypeEnum_v<LT>) {
					out.push_back((LT*)&*l);
				}
			}
		}

		template<typename LT>
		LT* FindLayer(std::vector<LT*> ls, std::string_view const& name) {
			for (auto& l : ls) {
				if (l->name == name) return l;
			}
			return nullptr;
		}
	}

}
