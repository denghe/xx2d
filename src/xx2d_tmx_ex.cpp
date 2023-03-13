﻿#include "xx2d.h"

namespace xx {

	namespace TMX {

		void Camera::Init(XY const& screenSize, Map& map) {
			tileWidth = map.tileWidth;
			tileHeight = map.tileHeight;

			worldRowCount = map.height;
			worldColumnCount = map.width;

			worldPixel.x = tileWidth * worldColumnCount;
			worldPixel.y = tileHeight * worldRowCount;

			this->screenSize = screenSize;

			Commit();
		}

		void Camera::Commit() {
			if (!dirty) return;
			dirty = false;

			auto halfNumRows = screenSize.y / scale.y / tileHeight / 2;
			int32_t posRowIndex = pos.y / tileHeight;
			rowFrom = posRowIndex - halfNumRows;
			rowTo = posRowIndex + halfNumRows + 2;
			if (rowFrom < 0) {
				rowFrom = 0;
			}
			if (rowTo > worldRowCount) {
				rowTo = worldRowCount;
			}

			auto halfNumColumns = screenSize.x / scale.x / tileWidth / 2;
			int32_t posColumnIndex = pos.x / tileWidth;
			columnFrom = posColumnIndex - halfNumColumns;
			columnTo = posColumnIndex + halfNumColumns + 2;
			if (columnFrom < 0) {
				columnFrom = 0;
			}
			if (columnTo > worldColumnCount) {
				columnTo = worldColumnCount;
			}

			at = at.MakePosScale(XY{ -pos.x, pos.y } *scale, scale);
		}

		void Camera::SetScale(float const& scale) {
			this->scale = { scale, scale };
			dirty = true;
		}

		void Camera::SetScreenSize(XY const& wh) {
			this->screenSize = wh;
			dirty = true;
		}

		void Camera::SetPosition(XY const& xy) {
			this->pos = xy;
			dirty = true;
		}

		void Camera::SetPositionX(float const& x) {
			this->pos.x = x;
			dirty = true;
		}

		void Camera::SetPositionY(float const& y) {
			this->pos.y = y;
			dirty = true;
		}

	}

}
