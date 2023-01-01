#pragma once
#include "pch.h"

// tiled map xml version data loader & container. supported to version 1.9x. compress method only support zstandard
// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#

// todo: double int uint check
// todo: sort field by editor UI
// todo: + if exists check
// todo: set fields default value
// todo: sort tiledset by firstgid ?
// todo: texture ? frame ?
// todo: gid to frame ?
// todo: strong type refine
// todo: fill step 2: ptrs by id, props obj by id
// todo: vector reserve
// todo: + field: "class"
// todo: handle path "../../". remove rootDir ( write global function for append path? test FS::path )

struct TMXData {

	template<typename ET>
	struct StrToEnumMappings {
		inline static constexpr std::string_view const* svs = nullptr;
		inline static constexpr size_t count = 0;
	};

	/**********************************************************************************/

	enum class Orientations : uint8_t {
		Orthogonal,
		Isometric,
		Staggered,	// not for Tileset
		Hexagonal,	// not for Tileset
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 4> enumTexts_Orientations = {
		"orthogonal"sv,
		"isometric"sv,
		"staggered"sv,
		"hexagonal"sv
	};
	template<>
	struct StrToEnumMappings<Orientations> {
		inline static constexpr std::string_view const* svs = enumTexts_Orientations.data();
		inline static constexpr size_t count = enumTexts_Orientations.size();
	};

	/**********************************************************************************/

	enum class RenderOrders : uint8_t {
		RightDown,
		RightUp,
		LeftDown,
		LeftUp,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 4> enumTexts_RenderOrders = {
		"right-down"sv,
		"right-up"sv,
		"left-down"sv,
		"left-up"sv
	};
	template<>
	struct StrToEnumMappings<RenderOrders> {
		inline static constexpr std::string_view const* svs = enumTexts_RenderOrders.data();
		inline static constexpr size_t count = enumTexts_RenderOrders.size();
	};

	/**********************************************************************************/

	enum class WangSetTypes : uint8_t {
		Corner,
		Edge,
		Mixed,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 3> enumTexts_WangSetTypes = {
		"corner"sv,
		"edge"sv,
		"mixed"sv,
	};
	template<>
	struct StrToEnumMappings<WangSetTypes> {
		inline static constexpr std::string_view const* svs = enumTexts_WangSetTypes.data();
		inline static constexpr size_t count = enumTexts_WangSetTypes.size();
	};

	/**********************************************************************************/

	enum class Encodings : uint8_t {
		Csv,
		Base64,													// support Compressions
		Xml,													// deprecated
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 3> enumTexts_Encodings = {
		"csv"sv,
		"base64"sv,
		"xml"sv,
	};
	template<>
	struct StrToEnumMappings<Encodings> {
		inline static constexpr std::string_view const* svs = enumTexts_Encodings.data();
		inline static constexpr size_t count = enumTexts_Encodings.size();
	};

	/**********************************************************************************/

	enum class Compressions : uint8_t {
		Uncompressed,
		Gzip,
		Zlib,
		Zstd,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 4> enumTexts_Compressions = {
		"uncompressed"sv,
		"gzip"sv,
		"zlib"sv,
		"zstd"sv,
	};
	template<>
	struct StrToEnumMappings<Compressions> {
		inline static constexpr std::string_view const* svs = enumTexts_Compressions.data();
		inline static constexpr size_t count = enumTexts_Compressions.size();
	};

	/**********************************************************************************/

	enum class HAligns : uint8_t {
		Left,
		Center,
		Right,
		Justify,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 4> enumTexts_HAligns = {
		"left"sv,
		"center"sv,
		"right"sv,
		"justify"sv,
	};
	template<>
	struct StrToEnumMappings<HAligns> {
		inline static constexpr std::string_view const* svs = enumTexts_HAligns.data();
		inline static constexpr size_t count = enumTexts_HAligns.size();
	};

	/**********************************************************************************/

	enum class StaggerAxiss : uint8_t {
		X,
		Y,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 2> enumTexts_StaggerAxiss = {
		"x"sv,
		"y"sv,
	};
	template<>
	struct StrToEnumMappings<StaggerAxiss> {
		inline static constexpr std::string_view const* svs = enumTexts_StaggerAxiss.data();
		inline static constexpr size_t count = enumTexts_StaggerAxiss.size();
	};

	/**********************************************************************************/

	enum class StaggerIndexs : uint8_t {
		Odd,
		Even,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 2> enumTexts_StaggerIndexs = {
		"odd"sv,
		"even"sv,
	};
	template<>
	struct StrToEnumMappings<StaggerIndexs> {
		inline static constexpr std::string_view const* svs = enumTexts_StaggerIndexs.data();
		inline static constexpr size_t count = enumTexts_StaggerIndexs.size();
	};

	/**********************************************************************************/

	enum class VAligns : uint8_t {
		Top,
		Center,
		Bottom,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 3> enumTexts_VAligns = {
		"top"sv,
		"center"sv,
		"bottom"sv,
	};
	template<>
	struct StrToEnumMappings<VAligns> {
		inline static constexpr std::string_view const* svs = enumTexts_VAligns.data();
		inline static constexpr size_t count = enumTexts_VAligns.size();
	};


	/**********************************************************************************/

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
	inline static constexpr std::array<std::string_view, 7> enumTexts_PropertyTypes = {
		"bool"sv,
		"color"sv,
		"float"sv,
		"file"sv,
		"int"sv,
		"object"sv,
		"string"sv,
	};
	template<>
	struct StrToEnumMappings<PropertyTypes> {
		inline static constexpr std::string_view const* svs = enumTexts_PropertyTypes.data();
		inline static constexpr size_t count = enumTexts_PropertyTypes.size();
	};

	/**********************************************************************************/

	enum class DrawOrders : uint8_t {
		TopDown,
		Index,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 2> enumTexts_DrawOrders = {
		"topdown"sv,
		"index"sv,
	};
	template<>
	struct StrToEnumMappings<DrawOrders> {
		inline static constexpr std::string_view const* svs = enumTexts_DrawOrders.data();
		inline static constexpr size_t count = enumTexts_DrawOrders.size();
	};

	/**********************************************************************************/

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
	inline static constexpr std::array<std::string_view, 10> enumTexts_ObjectAlignment = {
		"unspecified"sv,
		"topleft"sv,
		"top"sv,
		"topright"sv,
		"left"sv,
		"center"sv,
		"right"sv,
		"bottomleft"sv,
		"bottom"sv,
		"bottomright"sv,
	};
	template<>
	struct StrToEnumMappings<ObjectAlignment> {
		inline static constexpr std::string_view const* svs = enumTexts_ObjectAlignment.data();
		inline static constexpr size_t count = enumTexts_ObjectAlignment.size();
	};

	/**********************************************************************************/

	enum class TileRenderSizes : uint8_t {
		TileSize,
		MapGridSize,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 2> enumTexts_TileRenderSizes = {
		"tile"sv,
		"grid"sv,
	};
	template<>
	struct StrToEnumMappings<TileRenderSizes> {
		inline static constexpr std::string_view const* svs = enumTexts_TileRenderSizes.data();
		inline static constexpr size_t count = enumTexts_TileRenderSizes.size();
	};

	/**********************************************************************************/

	enum class FillModes : uint8_t {
		Stretch,
		PreserveAspectFit,
		MAX_VALUE_UNKNOWN
	};
	inline static constexpr std::array<std::string_view, 2> enumTexts_FillModes = {
		"stretch"sv,
		"preserve-aspect-fit"sv,
	};
	template<>
	struct StrToEnumMappings<FillModes> {
		inline static constexpr std::string_view const* svs = enumTexts_FillModes.data();
		inline static constexpr size_t count = enumTexts_FillModes.size();
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

	/**********************************************************************************/

	enum class LayerTypes : uint8_t {
		Tile,
		Object,
		Image,
		Group,
		MAX_VALUE_UNKNOWN
	};

	/**********************************************************************************/


	struct Property {
		PropertyTypes type = PropertyTypes::MAX_VALUE_UNKNOWN;
		std::string name;
	};

	struct Property_Bool : Property {
		bool value = false;
	};

	struct Property_Color : Property {
		RGBA8 value = { 0, 0, 0, 255 };
	};

	struct Property_Float : Property {
		double value = 0;
	};

	struct Property_String : Property {
		std::string value;
	};

	struct Property_File : Property_String {
	};

	struct Property_Int : Property {
		int64_t value = 0;
	};

	struct Object;
	struct Property_Object : Property {
		xx::Shared<Object> value;
		uint32_t objectId = 0;
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
		std::vector<xx::Shared<Property>> properties;
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

	struct Tileset;
	struct Object_Tile : Object_Rectangle {
		uint32_t gid = 0;				// & 0x3FFFFFFFu;
		bool flippingHorizontal = false;	// (gid >> 31) & 1
		bool flippingVertical = false;		// (gid >> 30) & 1
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
		std::vector<xx::Shared<Property>> properties;
	};

	struct Chunk {
		std::vector<uint32_t> gids;
		uint32_t height = 0;
		uint32_t width = 0;
		Pointi xy;	// x, y
	};

	struct Layer_Tile : Layer {
		std::vector<Chunk> chunks;	// when map.infinite == true
		std::vector<uint32_t> gids;	// when map.infinite == false
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

	struct Layer_Image {
		xx::Shared<Image> image;
		std::optional<RGBA8> transparentColor;	// trans="ff00ff"
		bool repeatX = false;	// repeatx
		bool repeatY = false;	// repeaty
	};

	//struct Terrain {
	//	std::string name;
	//	uint32_t tile;
	//	std::vector<xx::Shared<Property>> properties;
	//};

	//struct AFrame {
	//	uint32_t duration;
	//	uint32_t tileid;
	//};

	//struct Tile {
	//	std::vector<AFrame> animation;
	//	uint32_t id;
	//	std::string image;
	//	uint32_t imageheight;
	//	uint32_t imagewidth;
	//	Layer objectgroup;
	//	double probability;
	//	std::vector<xx::Shared<Property>> properties;
	//	std::vector<uint32_t> terrain;
	//	std::string type;
	//};

	struct WangTile {
		uint32_t tileId = 0;	// tileid
		std::vector<uint8_t> wangIds;	// wangid
	};

	struct WangColor {
		std::string name;
		RGBA8 color = { 0, 0, 0, 255 };
		uint32_t tile = 0;
		double probability = 1;
		std::vector<xx::Shared<Property>> properties;
	};

	struct WangSet {
		std::string name;
		WangSetTypes type = WangSetTypes::Corner;
		uint32_t tile = 0;
		std::vector<WangTile> wangTiles;	// <wangtile
		std::vector<WangColor> wangColors;	// <wangcolor
		std::vector<xx::Shared<Property>> properties;
	};

	struct Transformations {
		bool flipHorizontally = false;	// hflip
		bool flipVertically = false;	// vflip
		bool rotate = false;
		bool preferUntransformedTiles = false;	// preferuntransformed
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
		std::vector<xx::Shared<Property>> properties;

		std::string version;
		std::string tiledversion;
		uint32_t tilecount = 0;

		std::vector<WangSet> wangSets;	// <wangsets>

		// todo
		//std::vector<Terrain> terrains;
		//std::vector<Tile> tiles;
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
		std::vector<xx::Shared<Property>> properties;

		std::string version;
		std::string tiledVersion;	// tiledversion
		uint32_t nextLayerId = 0;	// nextlayerid
		uint32_t nextObjectId = 0;	// nextobjectid
		std::vector<xx::Shared<Tileset>> tilesets;
		std::vector<xx::Shared<Layer>> layers;
		std::vector<xx::Shared<Image>> images;
	};


	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/

	Map map;

	// fill map data by .tmx file
	int Fill(Engine* const& eg, std::string_view const& tmxfn);

	// todo: get texture & rect info by gid for generate quad. looks like 
};
