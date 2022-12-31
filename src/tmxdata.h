#pragma once
#include "pch.h"

// tiled map xml version data loader & container. supported to version 1.9x. compress method only support zstandard
// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#

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

	enum class Orientations {
		Orthogonal,
		Isometric,
		Staggered,
		Hexagonal,
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

	enum class RenderOrders {
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

	enum class WangSetTypes {
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

	enum class Encodings {
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

	enum class Compressions {
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

	enum class HAligns {
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

	enum class StaggerAxiss {
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

	enum class StaggerIndexs {
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

	enum class VAligns {
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

	enum class PropertyTypes {
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

	enum class DrawOrders {
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

	enum class ObjectTypes {
		Point,
		Ellipse,
		Polygon,
		Rectangle,
		Tile,
		Text,
		MAX_VALUE_UNKNOWN
	};

	/**********************************************************************************/


	// Chunks are used to store the tile layer data for infinite maps.
	struct Chunk {
		// Array of unsigned int (GIDs) or base64-encoded data
		std::vector<uint32_t> gids;

		// Height in tiles
		int height;

		// Width in tiles
		int width;

		// X coordinate in tiles
		int x;

		// Y coordinate in tiles
		int y;
	};


	struct Property {
		std::string name;
		PropertyTypes type = PropertyTypes::MAX_VALUE_UNKNOWN;
	};

	struct PropertyBool : Property {
		bool value;
	};

	struct PropertyColor : Property {
		RGBA8 value;
	};

	struct PropertyFloat : Property {
		double value;
	};

	struct PropertyString : Property {
		std::string value;
	};

	struct PropertyFile : PropertyString {
	};

	struct PropertyInt : Property {
		int64_t value;
	};

	struct Object;
	struct PropertyObject : Property {
		uint32_t objectId = 0;
		xx::Shared<Object> value;
	};


	// A point on a polygon or a polyline, relative to the position of the object.
	struct Point {
		// X coordinate in pixels
		int32_t x = 0;

		// Y coordinate in pixels
		int32_t y = 0;
	};

	struct Object {
		ObjectTypes type = ObjectTypes::MAX_VALUE_UNKNOWN;

		// Incremental ID, unique across all objects
		uint32_t id = 0;

		// String assigned to name field in editor
		std::string name;

		// String assigned to class field in editor
		std::string class_;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// 	X coordinate in pixels
		double x = 0;

		// Y coordinate in pixels
		double y = 0;

		// Angle in degrees clockwise
		double rotation = false;

		// Whether object is shown in editor.
		bool visible = true;
	};

	struct ObjectPoint : Object {
	};

	struct ObjectRectangle : Object {
		// Width in pixels.
		uint32_t width = 0;

		// Height in pixels.
		uint32_t height = 0;
	};

	struct ObjectEllipse : ObjectRectangle {
	};

	struct ObjectPolygon : Object {
		// Array of Points
		std::vector<Point> points;
	};

	struct Tileset;
	struct ObjectTile : ObjectRectangle {
		// todo: offical doc
		bool flippingHorizontal;	// (gid >> 31) & 1

		// todo: offical doc
		bool flippingVertical;		// (gid >> 30) & 1

		// Global tile ID
		uint32_t gid;				// & 0x3FFFFFFFu;

		// gid owner
		//xx::Shared<Tileset> tileset;
	};

	struct ObjectText : ObjectRectangle {
		// todo: offical doc
		std::string text;
		std::string fontfamily;
		uint32_t pixelsize = 16;
		bool wrap = false;
		RGBA8 color = { 0, 0, 0, 255 };
		bool bold = false;
		bool italic = false;
		bool underline = false;
		bool strikeout = false;
		bool kerning = true;
		HAligns halign = HAligns::Left;
		VAligns valign = VAligns::Top;
	};

	struct ObjectTemplate : Object {
		// Reference to a template file, in case object is a template instance
		std::string template_;
	};

	// todo: offical doc here
	struct ObjectGroup {
		uint32_t id;
		std::string name;
		std::string class_;
		bool locked;
		bool visible;
		// topdown (default) or index. objectgroup only.
		DrawOrders draworder;
		double opacity;
		double offsetx;
		double offsety;
		double parallaxx;
		double parallaxy;
		std::optional<RGBA8> tintcolor;
		std::vector<xx::Shared<Object>> objects;
	};

	// todo: split to strong type
	struct Layer {
		// 	Array of chunks (optional). tilelayer only.
		std::vector<Chunk> chunks;

		// zlib, gzip, zstd (since Tiled 1.3) or empty (default). tilelayer only.
		Compressions compression;

		// Array of unsigned int (GIDs) or base64-encoded data. tilelayer only.
		std::vector<uint32_t> gids;

		// csv (default) or base64. tilelayer only.
		Encodings encoding;

		// Row count. Same as map height for fixed-size maps.
		int height;

		// Incremental ID - unique across all layers
		uint32_t id;

		// Image used by this layer. imagelayer only.
		std::string image;

		// Array of layers. group only.
		std::vector<Layer> layers;

		// Name assigned to this layer
		std::string name;

		// Array of objects. objectgroup only.
		std::vector<Object> objects;

		// Horizontal layer offset in pixels (default: 0)
		double offsetx;

		// Vertical layer offset in pixels (default: 0)
		double offsety;

		// Value between 0 and 1
		double opacity;

		// Horizontal parallax factor for this layer (default: 1). (since Tiled 1.5)
		double parallaxx;

		// Vertical parallax factor for this layer (default: 1). (since Tiled 1.5)
		double parallaxy;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// X coordinate where layer content starts (for infinite maps)
		int startx;

		// Y coordinate where layer content starts (for infinite maps)
		int starty;

		// Hex-formatted tint color (#RRGGBB or #AARRGGBB) that is multiplied with any graphics drawn by this layer or any child layers (optional).
		std::optional<RGBA8> tintcolor;

		// Hex-formatted color (#RRGGBB) (optional). imagelayer only.
		std::optional<RGBA8> transparentcolor;

		// tilelayer, objectgroup, imagelayer or group
		std::string type;

		// Whether layer is shown or hidden in editor
		bool visible;

		// Column count. Same as map width for fixed-size maps.
		int width;

		// Horizontal layer offset in tiles. Always 0.
		int x;

		// Vertical layer offset in tiles. Always 0.
		int y;
	};

	// Specifies common grid settings used for tiles in a tileset. See <grid> in the TMX Map Format.
	struct Grid {
		// Cell height of tile grid
		int height;

		// orthogonal (default) or isometric
		Orientations orientation;

		// Cell width of tile grid
		int width;
	};

	struct Terrain {
		// Name of terrain
		std::string name;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// Local ID of tile representing terrain
		int tile;
	};

	// This element is used to specify an offset in pixels, to be applied when drawing a tile from the related tileset. When not present, no offset is applied.
	struct TileOffset {
		// Horizontal offset in pixels
		int x;

		// Vertical offset in pixels (positive is down)
		int y;
	};

	struct Frame {
		// Frame duration in milliseconds
		int duration;

		// Local tile ID representing this frame
		int tileid;
	};

	struct Tile {
		// Array of Frames
		std::vector<Frame> animation;

		// Local ID of the tile
		int id;

		// Image representing this tile (optional)
		std::string image;

		// Height of the tile image in pixels
		int imageheight;

		// Width of the tile image in pixels
		int imagewidth;

		// Layer with type objectgroup, when collision shapes are specified(optional)
		Layer objectgroup;

		// Percentage chance this tile is chosen when competing with others in the editor(optional)
		double probability;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// Index of terrain for each corner of tile (optional)
		std::vector<int> terrain;

		// The type of the tile(optional)
		std::string type;
	};

	// This element is used to describe which transformations can be applied to the tiles (e.g. to extend a Wang set by transforming existing tiles).
	struct Transformations {
		// Tiles can be flipped horizontally
		bool hflip;

		// Tiles can be flipped vertically
		bool vflip;

		// Tiles can be rotated in 90-degree increments
		bool rotate;

		// Whether untransformed tiles remain preferred, otherwise transformed tiles are used to produce more variations
		bool preferuntransformed;
	};

	struct WangTile {
		// Local ID of tile
		uint32_t tileId;

		// Array of Wang color indexes (uchar[8])
		std::vector<byte> wangid;
	};

	struct WangColor {
		// Hex-formatted color (#RRGGBB or #AARRGGBB)
		RGBA8 color;

		// Name of the Wang color
		std::string name;

		// Probability used when randomizing
		double probability;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// Local ID of tile representing the Wang color
		uint32_t tile;
	};

	struct WangSet {
		// Array of Wang colors
		std::vector<WangColor> colors;

		// Name of the Wang set
		std::string name;

		// Type of the Wang set: corner, edge, mixed
		WangSetTypes type;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// Local ID of tile representing the Wang set
		uint32_t tile;

		// Array of Wang tiles
		std::vector<WangTile> wangtiles;
	};

	struct Tileset {
		// Hex-formatted color (#RRGGBB or #AARRGGBB) (optional)
		std::optional<RGBA8> backgroundcolor;

		// todo: offical doc here
		std::string class_;

		// The number of tile columns in the tileset
		int columns;

		//	GID corresponding to the first tile in the set
		uint32_t firstgid;

		// (optional)
		Grid grid;

		// Image used for tiles in this set
		xx::Shared<GLTexture> image;

		// Height of source image in pixels
		int imageheight;

		// Width of source image in pixels
		int imagewidth;

		// Buffer between image edge and first tile (pixels)
		int margin;

		// Name given to this tileset
		std::string name;

		// Alignment to use for tile objects (unspecified (default), topleft, top, topright, left, center, right, bottomleft, bottom or bottomright) (since 1.4)
		std::string objectalignment;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// The external file that contains this tilesets data
		std::string source;

		// Spacing between adjacent tiles in image (pixels)
		int spacing;

		// Array of Terrains (optional)
		std::vector<Terrain> terrains;

		// The number of tiles in this tileset
		int tilecount;

		// The Tiled version used to save the file
		std::string tiledversion;

		// Maximum height of tiles in this set
		int tileheight;

		// (optional)
		TileOffset tileoffset;

		// Array of Tiles (optional)
		std::vector<Tile> tiles;

		// Maximum width of tiles in this set
		int tilewidth;

		// Allowed transformations (optional)
		Transformations transformations;

		// Hex-formatted color (#RRGGBB) (optional)
		std::optional<RGBA8> transparentcolor;

		// tileset (for tileset files, since 1.0)
		std::string type;

		// The JSON format version (previously a number, saved as string since 1.6)
		std::string version;

		// Array of Wang sets (since 1.1.5)
		std::vector<WangSet> wangsets;
	};

	struct Map {
		// Hex-formatted color (#RRGGBB or #AARRGGBB) (optional)
		std::optional<RGBA8> backgroundcolor;

		// The compression level to use for tile layer data (defaults to -1, which means to use the algorithm default)
		int compressionlevel;

		// todo: offical doc here
		uint32_t outputChunkWidth;

		// todo: offical doc here
		uint32_t outputChunkHeight;

		// todo: offical doc here
		std::string class_;

		// Number of tile rows
		uint32_t height;

		// 	Length of the side of a hex tile in pixels (hexagonal maps only)
		uint32_t hexsidelength;

		// 	Whether the map has infinite dimensions
		bool infinite;

		// Array of Layers
		std::vector<Layer> layers;

		// Auto-increments for each layer
		uint32_t nextlayerid;

		// 	Auto-increments for each placed object
		uint32_t nextobjectid;

		// orthogonal, isometric, staggered or hexagonal
		Orientations orientation;

		// todo: offical doc here
		std::vector<ObjectGroup> objectgroups;

		// todo: offical doc here
		double parallaxoriginx;

		// todo: offical doc here
		double parallaxoriginy;

		// Array of Properties
		std::vector<xx::Shared<Property>> properties;

		// right-down (the default), right-up, left-down or left-up (currently only supported for orthogonal maps)
		RenderOrders renderorder;

		// x or y (staggered / hexagonal maps only)
		StaggerAxiss staggeraxis;

		// odd or even (staggered / hexagonal maps only)
		StaggerIndexs staggerindex;

		// The Tiled version used to save the file (since Tiled 1.0.1). May be a date (for snapshot builds). (optional)
		std::string tiledversion;

		// Map grid height
		uint32_t tileheight;

		// Array of Tilesets
		std::vector<xx::Shared<Tileset>> tilesets;

		// Map grid width
		uint32_t tilewidth;

		// map (since 1.0)
		std::string type;

		// The TMX format version. Was “1.0” so far, and will be incremented to match minor Tiled releases.
		std::string version;

		// Number of tile columns
		uint32_t width;
	};


	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/

	Map map;

	// fill map data by .tmx file
	int Fill(Engine* const& eg, std::string_view const& tmxfn);
};
