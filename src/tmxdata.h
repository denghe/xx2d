﻿#pragma once
#include "pch.h"

// tiled map xml version data loader & container. supported to version 1.9x. compress method support zstandard
struct TMXData {

	// Chunks are used to store the tile layer data for infinite maps.
	struct Chunk {
		// Array of unsigned int (GIDs) or base64-encoded data
		std::string dataBase64;
		//std::vector<uint32_t> data;

		// Height in tiles
		int height;

		// Width in tiles
		int width;

		// X coordinate in tiles
		int x;

		// Y coordinate in tiles
		int y;
	};

	// A point on a polygon or a polyline, relative to the position of the object.
	struct Point {
		// X coordinate in pixels
		double x;

		// Y coordinate in pixels
		double y;
	};

	// Wraps any number of custom properties. Can be used as a child of the map, tileset, tile (when part of a tileset), terrain, wangset, wangcolor, layer, objectgroup, object, imagelayer and group elements.
	struct Property {
		// Name of the property
		std::string name;

		// The type of the property. Can be string (default), int, float, bool, color, file or object (since 0.16, with color and file added in 0.17, and object added in 1.4).
		std::string type;

		// The value of the property. (default string is “”, default number is 0, default boolean is “false”, default color is #00000000, default file is “.” (the current file’s parent directory))
		// Boolean properties have a value of either “true” or “false”.
		// Color properties are stored in the format #AARRGGBB.
		// File properties are stored as paths relative from the location of the map file.
		// Object properties can reference any object on the same map and are stored as an integer (the ID of the referenced object, or 0 when no object is referenced). When used on objects in the Tile Collision Editor, they can only refer to other objects on the same tile.
		// When a string property contains newlines, the current version of Tiled will write out the value as characters contained inside the property element rather than as the value attribute. It is possible that a future version of the TMX format will switch to always saving property values inside the element rather than as an attribute.
		std::string value;
	};

	struct Object {
		// Used to mark an object as an ellipse
		bool ellipse;

		// Global tile ID, only if object represents a tile
		int gid;

		// Height in pixels.
		double height;

		// Incremental ID, unique across all objects
		int id;

		// String assigned to name field in editor
		std::string name;

		// Used to mark an object as a point
		bool point;

		// Array of Points, in case the object is a polygon
		std::vector<Point> polygon;

		// Array of Points, in case the object is a polyline
		std::vector<Point> polyline;

		// Array of Properties
		std::vector<Property> properties;

		// Angle in degrees clockwise
		double rotation;

		// Reference to a template file, in case object is a template instance
		std::string template_;

		// Only used for text objects
		std::string text;

		// String assigned to type field in editor
		std::string type;

		// Whether object is shown in editor.
		bool visible;

		// Width in pixels.
		double width;

		// 	X coordinate in pixels
		double x;

		// Y coordinate in pixels
		double y;
	};

	struct Layer {
		// 	Array of chunks (optional). tilelayer only.
		std::vector<Chunk> chunks;

		// zlib, gzip, zstd (since Tiled 1.3) or empty (default). tilelayer only.
		std::string compression;

		// Array of unsigned int (GIDs) or base64-encoded data. tilelayer only.
		std::string dataBase64;
		//std::vector<uint32_t> data;

		// topdown (default) or index. objectgroup only.
		std::string draworder;

		// 	csv (default) or base64. tilelayer only.
		std::string encoding;

		// Row count. Same as map height for fixed-size maps.
		int height;

		// Incremental ID - unique across all layers
		int id;

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
		std::vector<Property> properties;

		// X coordinate where layer content starts (for infinite maps)
		int startx;

		// Y coordinate where layer content starts (for infinite maps)
		int starty;

		// Hex-formatted tint color (#RRGGBB or #AARRGGBB) that is multiplied with any graphics drawn by this layer or any child layers (optional).
		std::string tintcolor;

		// Hex-formatted color (#RRGGBB) (optional). imagelayer only.
		std::string transparentcolor;

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
		std::string orientation;

		// Cell width of tile grid
		int width;
	};

	struct Terrain {
		// Name of terrain
		std::string name;

		// Array of Properties
		std::vector<Property> properties;

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
		std::vector<Property> properties;

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
		int tileId;

		// Array of Wang color indexes (uchar[8])
		std::vector<byte> wangid;
	};

	struct WangColor {
		// Hex-formatted color (#RRGGBB or #AARRGGBB)
		std::string color;

		// Name of the Wang color
		std::string name;

		// Probability used when randomizing
		double probability;

		// Array of Properties
		std::vector<Property> properties;

		// Local ID of tile representing the Wang color
		int tile;
	};

	struct WangSet {
		// Array of Wang colors
		std::vector<WangColor> colors;

		// Name of the Wang set
		std::string name;

		// Array of Properties
		std::vector<Property> properties;

		// Local ID of tile representing the Wang set
		int tile;

		// Array of Wang tiles
		std::vector<WangTile> wangtiles;
	};

	struct Tileset {
		// Hex-formatted color (#RRGGBB or #AARRGGBB) (optional)
		std::string backgroundcolor;

		// The number of tile columns in the tileset
		int columns;

		//	GID corresponding to the first tile in the set
		int firstgid;

		// (optional)
		Grid grid;

		// Image used for tiles in this set
		std::string image;

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
		std::vector<Property> properties;

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
		std::string transparentcolor;

		// tileset (for tileset files, since 1.0)
		std::string type;

		// The JSON format version (previously a number, saved as string since 1.6)
		std::string version;

		// Array of Wang sets (since 1.1.5)
		std::vector<WangSet> wangsets;
	};

	struct Map {
		// Hex-formatted color (#RRGGBB or #AARRGGBB) (optional)
		std::string backgroundcolor;

		// The compression level to use for tile layer data (defaults to -1, which means to use the algorithm default)
		int compressionlevel;

		// Number of tile rows
		int height;

		// 	Length of the side of a hex tile in pixels (hexagonal maps only)
		int hexsidelength;

		// 	Whether the map has infinite dimensions
		bool infinite;

		// Array of Layers
		std::vector<Layer> layers;

		// Auto-increments for each layer
		int nextlayerid;

		// 	Auto-increments for each placed object
		int nextobjectid;

		// orthogonal, isometric, staggered or hexagonal
		std::string orientation;

		// Array of Properties
		std::vector<Property> properties;

		// right-down (the default), right-up, left-down or left-up (currently only supported for orthogonal maps)
		std::string renderorder;

		// x or y (staggered / hexagonal maps only)
		std::string staggeraxis;

		// odd or even (staggered / hexagonal maps only)
		std::string staggerindex;

		// The Tiled version used to save the file (since Tiled 1.0.1). May be a date (for snapshot builds). (optional)
		std::string tiledversion;

		// Map grid height
		int tileheight;

		// Array of Tilesets
		std::vector<Tileset> tilesets;

		// Map grid width
		int tilewidth;

		// map (since 1.0)
		std::string type;

		// The TMX format version. Was “1.0” so far, and will be incremented to match minor Tiled releases.
		std::string version;

		// Number of tile columns
		int width;
	};


	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/

	Map map;

	// 从 .tmx 文件内容加载数据到 map 变量
	int Fill(std::string_view text);
};