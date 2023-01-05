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
}
