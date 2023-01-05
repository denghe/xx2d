#include "pch.h"

namespace TMX {

	void Camera::Init(Size const& screenSize, Map& map) {
		tileWidth = map.tileWidth;
		tileHeight = map.tileHeight;

		worldRowCount = map.height;
		worldColumnCount = map.width;

		worldPixel.w = tileWidth * worldColumnCount;
		worldPixel.h = tileHeight * worldRowCount;

		this->screenSize = screenSize;

		Commit();
	}

	void Camera::Commit() {
		if (!dirty) return;
		dirty = false;

		// todo: scale

		auto halfNumRows = screenSize.h / tileHeight / 2;
		int32_t posRowIndex = pos.y / tileWidth;
		rowFrom = posRowIndex - halfNumRows;
		rowTo = posRowIndex + halfNumRows + 2;
		if (rowFrom < 0) {
			rowFrom = 0;
		}
		if (rowTo > worldRowCount) {
			rowTo = worldRowCount;
		}

		auto halfNumColumns = screenSize.w / tileWidth / 2;
		int32_t posColumnIndex = pos.x / tileHeight;
		columnFrom = posColumnIndex - halfNumColumns;
		columnTo = posColumnIndex + halfNumColumns + 1;
		if (columnFrom < 0) {
			columnFrom = 0;
		}
		if (columnTo > worldColumnCount) {
			columnTo = worldColumnCount;
		}

		offset = { -pos.x, pos.y };
	}

	void Camera::SetScale(float const& scale) {
		this->scale = { scale, scale };
		dirty = true;
	}

	void Camera::SetScreenSize(Size const& wh) {
		this->screenSize = wh;
		dirty = true;
	}

	void Camera::SetPos(XY const& xy) {
		this->pos = xy;
		dirty = true;
	}


	void Fill(std::vector<Sprite>& ss, Map const& map, Layer_Tile const& lt) {
		// convert all gids to sprites
		ss.resize(lt.gids.size());
		for (int cy = 0; cy < (int)map.height; ++cy) {
			for (int cx = 0; cx < (int)map.width; ++cx) {
				auto&& idx = cy * (int)map.width + cx;
				auto&& gid = lt.gids[idx];
				assert(gid < map.gidInfos.size());
				auto&& i = map.gidInfos[gid];
				assert(i.image);
				auto f = xx::Make<::Frame>();
				f->tex = i.image->texture;
				f->anchor = { 0, 1 };
				f->spriteSize = { (float)i.w, (float)i.h };
				f->textureRect = { (float)i.u, (float)i.v, (float)i.w, (float)i.h };
				auto& s = ss[idx];
				s.SetTexture(std::move(f));
				s.SetColor({ 255, 255, 255, 255 });
				s.SetScale({ 1, 1 });
				s.SetPositon({ float(cx * (int)map.tileWidth), float(-cy * (int)map.tileHeight) });
				s.Commit();
			}
		}
	}

	void Camera::Draw(Engine* eg, std::vector<Sprite>& ss) {
		for (uint32_t y = rowFrom; y < rowTo; ++y) {
			for (uint32_t x = columnFrom; x < columnTo; ++x) {
				auto&& s = ss[y * worldColumnCount + x];
				s.Draw(eg, *this);
			}
		}
	}
}
