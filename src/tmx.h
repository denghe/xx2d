#pragma once
#include "pch.h"

// tiled map xml version data loader & container. full supported to version 1.9x( compress algorithm only support zstandard )
// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#

struct Engine;
namespace TMX {

	enum class PropertyTypes : uint8_t {
		Bool,
		Color,
		Float,
		File,
		Int,
		Object,
		String,
		MAX_VALUE_UNKNOWN
	};

	struct Object;
	struct Property {
		PropertyTypes type = PropertyTypes::MAX_VALUE_UNKNOWN;
		std::string name;
		std::variant<bool, RGBA8, int64_t, double, std::unique_ptr<std::string>, Object*> value;
	};

	/**********************************************************************************/

	enum class ObjectTypes : uint8_t {
		Point,
		Ellipse,
		Polygon,
		Rectangle,
		Tile,
		Text,
		MAX_VALUE_UNKNOWN
	};

	struct Object {
		ObjectTypes type = ObjectTypes::MAX_VALUE_UNKNOWN;
		uint32_t id = 0;
		std::string name;
		std::string class_;
		double x = 0;
		double y = 0;
		double rotation = false;
		bool visible = true;
		std::vector<Property> properties;	// <properties> <property <property
	};

	struct Object_Point : Object {
	};

	struct Object_Rectangle : Object {
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct Object_Ellipse : Object_Rectangle {
	};

	struct Pointi {
		int32_t x = 0, y = 0;
	};

	struct Pointf {
		double x = 0, y = 0;
	};

	struct Object_Polygon : Object {
		std::vector<Pointi> points;
	};

	struct Object_Tile : Object_Rectangle {
		uint32_t gid = 0;					// & 0x3FFFFFFFu;
		bool flippingHorizontal = false;	// (gid >> 31) & 1
		bool flippingVertical = false;		// (gid >> 30) & 1
	};

	enum class HAligns : uint8_t {
		Left,
		Center,
		Right,
		Justify,
		MAX_VALUE_UNKNOWN
	};

	enum class VAligns : uint8_t {
		Top,
		Center,
		Bottom,
		MAX_VALUE_UNKNOWN
	};

	struct Object_Text : Object_Rectangle {
		std::string fontfamily;
		uint32_t pixelsize = 16;
		RGBA8 color = { 0, 0, 0, 255 };
		bool wrap = false;
		bool bold = false;
		bool italic = false;
		bool underline = false;
		bool strikeout = false;
		bool kerning = true;
		HAligns halign = HAligns::Left;
		VAligns valign = VAligns::Top;
		std::string text;
	};

	/**********************************************************************************/

	enum class LayerTypes : uint8_t {
		TileLayer,
		ObjectLayer,
		ImageLayer,
		GroupLayer,
		MAX_VALUE_UNKNOWN
	};

	struct Layer {
		LayerTypes type = LayerTypes::MAX_VALUE_UNKNOWN;
		uint32_t id = 0;
		std::string name;
		std::string class_;	// class
		bool visible = true;
		bool locked = false;
		double opacity = 1;	// 0 ~ 1
		std::optional<RGBA8> tintColor;	// tintcolor
		double horizontalOffset = 0;	// offsetx
		double verticalOffset = 0;	// offsety
		Pointf parallaxFactor = {1, 1};	// parallaxx, parallaxy
		std::vector<Property> properties;	// <properties> <property <property
	};

	struct Chunk {
		std::vector<uint32_t> gids;
		uint32_t height = 0;
		uint32_t width = 0;
		Pointi pos;	// x, y
	};

	struct Layer_Tile : Layer {
		std::vector<Chunk> chunks;	// when map.infinite == true
		std::vector<uint32_t> gids;	// when map.infinite == false
	};

	enum class DrawOrders : uint8_t {
		TopDown,
		Index,
		MAX_VALUE_UNKNOWN
	};

	struct Layer_Object : Layer {
		std::optional<RGBA8> color;
		DrawOrders draworder = DrawOrders::TopDown;
		std::vector<xx::Shared<Object>> objects;
	};

	struct Layer_Group : Layer {
		std::vector<xx::Shared<Layer>> layers;
	};

	struct Image {
		std::string source;
		uint32_t width;
		uint32_t height;
		std::optional<RGBA8> transparentColor;	// trans="ff00ff"
		xx::Shared<GLTexture> texture;
	};

	struct Layer_Image : Layer {
		xx::Shared<Image> image;
		bool repeatX = false;	// repeatx
		bool repeatY = false;	// repeaty
	};

	/**********************************************************************************/

	struct Frame {
		uint32_t tileId = 0;	// tileid
		uint32_t gid = 0;
		uint32_t duration = 0;
	};
	struct Tile {
		uint32_t id = 0;
		std::string class_;
		xx::Shared<Image> image;	// <image ...>
		xx::Shared<Layer_Object> collisions;	// <objectgroup> <object <object
		std::vector<Frame> animation;	// <animation> <frame <frame
		std::vector<Property> properties;	// <properties> <property <property
	};

	struct WangTile {
		uint32_t tileId = 0;	// tileid
		uint32_t gid = 0;
		std::vector<uint8_t> wangIds;	// wangid
	};

	struct WangColor {
		std::string name;
		RGBA8 color = { 0, 0, 0, 255 };
		uint32_t tile = 0;
		double probability = 1;
		std::vector<Property> properties;	// <properties> <property <property
	};

	enum class WangSetTypes : uint8_t {
		Corner,
		Edge,
		Mixed,
		MAX_VALUE_UNKNOWN
	};

	struct WangSet {
		std::string name;
		WangSetTypes type = WangSetTypes::Corner;
		uint32_t tile = 0;
		std::vector<WangTile> wangTiles;	// <wangtile
		std::vector<std::unique_ptr<WangColor>> wangColors;	// <wangcolor
		std::vector<Property> properties;	// <properties> <property <property
	};

	struct Transformations {
		bool flipHorizontally = false;	// hflip
		bool flipVertically = false;	// vflip
		bool rotate = false;
		bool preferUntransformedTiles = false;	// preferuntransformed
	};

	enum class Orientations : uint8_t {
		Orthogonal,
		Isometric,
		Staggered,	// not for Tileset
		Hexagonal,	// not for Tileset
		MAX_VALUE_UNKNOWN
	};

	enum class ObjectAlignment : uint8_t {
		Unspecified,
		TopLeft,
		Top,
		TopRight,
		Left,
		Center,
		Right,
		BottomLeft,
		Bottom,
		BottomRight,
		MAX_VALUE_UNKNOWN
	};

	enum class TileRenderSizes : uint8_t {
		TileSize,
		MapGridSize,
		MAX_VALUE_UNKNOWN
	};

	enum class FillModes : uint8_t {
		Stretch,
		PreserveAspectFit,
		MAX_VALUE_UNKNOWN
	};

	struct Tileset {
		uint32_t firstgid;	// .tmx map/tileset.firstgid
		std::string source;	// .tmx map/tileset.source

		// following fields in source .tsx file
		std::string name;
		std::string class_;	// class
		ObjectAlignment objectAlignment = ObjectAlignment::Unspecified;	// objectalignment
		Pointi drawingOffset;	// <tileoffset x= y=
		TileRenderSizes tileRenderSize = TileRenderSizes::TileSize; // tilerendersize
		FillModes fillMode = FillModes::Stretch;	// fillmode
		std::optional<RGBA8> backgroundColor;	// backgroundcolor
		Orientations orientation = Orientations::Orthogonal;	// <grid orientation=
		uint32_t gridWidth = 0;	// <grid width=
		uint32_t gridHeight = 0;	// <grid height=
		uint32_t columns = 0;
		Transformations allowedTransformations;	// <transformations ...
		xx::Shared<Image> image;
		uint32_t tilewidth = 0;
		uint32_t tileheight = 0;
		uint32_t margin = 0;
		uint32_t spacing = 0;
		std::vector<Property> properties;	// <properties> <property <property

		std::string version;
		std::string tiledversion;
		uint32_t tilecount = 0;

		std::vector<std::unique_ptr<WangSet>> wangSets;	// <wangsets>
		std::vector<std::unique_ptr<Tile>> tiles;	// <tile <tile <tile
	};

	/**********************************************************************************/

	enum class RenderOrders : uint8_t {
		RightDown,
		RightUp,
		LeftDown,
		LeftUp,
		MAX_VALUE_UNKNOWN
	};

	enum class Encodings : uint8_t {
		Csv,
		Base64,	// support Compressions
		Xml,	// deprecated
		MAX_VALUE_UNKNOWN
	};

	enum class Compressions : uint8_t {
		Uncompressed,
		Gzip,
		Zlib,
		Zstd,
		MAX_VALUE_UNKNOWN
	};

	enum class StaggerAxiss : uint8_t {
		X,
		Y,
		MAX_VALUE_UNKNOWN
	};

	enum class StaggerIndexs : uint8_t {
		Odd,
		Even,
		MAX_VALUE_UNKNOWN
	};

	// ext for fast search gid & get texture & quad data
	struct GidInfo {
		Tileset* tileset;
		Tile* tile;	// maybe nullptr
		Image* image;
		uint16_t u, v, w, h;	// uv box

		bool IsSingleImage() const;	// return image == tile->image
	};

	struct Map {
		std::string class_;	// class
		Orientations orientation = Orientations::Orthogonal;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t tileWidth = 0;	// tilewidth
		uint32_t tileHeight = 0;	// tileheight
		bool infinite = false;
		uint32_t tileSideLength = 0;	// hexsidelength
		StaggerAxiss staggeraxis = StaggerAxiss::Y;	// staggeraxis
		StaggerIndexs staggerindex = StaggerIndexs::Odd;	// staggerindex
		Pointf parallaxOrigin;	// parallaxoriginx, parallaxoriginy
		std::pair<Encodings, Compressions> tileLayerFormat = { Encodings::Csv,  Compressions::Uncompressed };	// for every layer's data
		uint32_t outputChunkWidth = 16;
		uint32_t outputChunkHeight = 16;
		RenderOrders renderOrder = RenderOrders::RightDown;	// renderorder
		int32_t compressionLevel = -1;	// compressionlevel
		std::optional<RGBA8> backgroundColor;	// backgroundcolor
		std::vector<Property> properties;	// <properties> <property <property

		std::string version;
		std::string tiledVersion;	// tiledversion
		uint32_t nextLayerId = 0;	// nextlayerid
		uint32_t nextObjectId = 0;	// nextobjectid
		std::vector<xx::Shared<Tileset>> tilesets;
		std::vector<xx::Shared<Layer>> layers;

		// ext
		std::vector<xx::Shared<Image>> images;
		std::vector<GidInfo> gidInfos;	// for search by index: gid
	};

	/**********************************************************************************/

	void FillTo(Map& map, Engine* eg, std::string_view const& tmxfn);
};
