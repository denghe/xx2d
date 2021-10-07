#pragma once
#include <xx_obj.h>
#include <tiledmap.h.inc>
struct CodeGen_tiledmap {
	inline static const ::std::string md5 = "#*MD5<c7c589d6fa61c1e1c5163d0b3b69fd86>*#";
    static void Register();
    CodeGen_tiledmap() { Register(); }
};
inline CodeGen_tiledmap __CodeGen_tiledmap;
namespace tiledmap_original {
    struct Property {
        XX_OBJ_STRUCT_H(Property)
        using IsSimpleType_v = Property;
        ::std::string name;
        ::std::string type;
        ::std::string value;
    };
}
namespace tiledmap_original {
    struct Point {
        XX_OBJ_STRUCT_H(Point)
        using IsSimpleType_v = Point;
        double x = 0;
        double y = 0;
    };
}
namespace tiledmap_original {
    struct Chunk {
        XX_OBJ_STRUCT_H(Chunk)
        using IsSimpleType_v = Chunk;
        ::std::vector<uint32_t> data;
        int32_t height = 0;
        int32_t width = 0;
        int32_t x = 0;
        int32_t y = 0;
    };
}
namespace tiledmap_original {
    struct Object {
        XX_OBJ_STRUCT_H(Object)
        using IsSimpleType_v = Object;
        bool ellipse = false;
        int32_t gid = 0;
        double height = 0;
        int32_t id = 0;
        ::std::string name;
        bool point = false;
        ::std::vector<::tiledmap_original::Point> polygon;
        ::std::vector<::tiledmap_original::Point> polyline;
        ::std::vector<::tiledmap_original::Property> properties;
        double rotation = 0;
        ::std::string template_;
        ::std::string text;
        ::std::string type;
        bool visible = false;
        double width = 0;
        double x = 0;
        double y = 0;
    };
}
namespace tiledmap_original {
    struct Layer {
        XX_OBJ_STRUCT_H(Layer)
        using IsSimpleType_v = Layer;
        ::std::vector<::tiledmap_original::Chunk> chunks;
        ::std::string compression;
        ::std::vector<uint32_t> data;
        ::std::string draworder;
        ::std::string encoding;
        int32_t height = 0;
        int32_t id = 0;
        ::std::string image;
        ::std::vector<::tiledmap_original::Layer> layers;
        ::std::string name;
        ::std::vector<::tiledmap_original::Object> objects;
        double offsetx = 0;
        double offsety = 0;
        double opacity = 0;
        double parallaxx = 0;
        double parallaxy = 0;
        ::std::vector<::tiledmap_original::Property> properties;
        int32_t startx = 0;
        int32_t starty = 0;
        ::std::string tintcolor;
        ::std::string transparentcolor;
        ::std::string type;
        bool visible = false;
        int32_t width = 0;
        int32_t x = 0;
        int32_t y = 0;
    };
}
namespace tiledmap_original {
    struct Frame {
        XX_OBJ_STRUCT_H(Frame)
        using IsSimpleType_v = Frame;
        int32_t duration = 0;
        int32_t tileid = 0;
    };
}
namespace tiledmap_original {
    struct WangTile {
        XX_OBJ_STRUCT_H(WangTile)
        using IsSimpleType_v = WangTile;
        int32_t tileId = 0;
        ::std::vector<uint8_t> wangid;
    };
}
namespace tiledmap_original {
    struct WangColor {
        XX_OBJ_STRUCT_H(WangColor)
        using IsSimpleType_v = WangColor;
        ::std::string color;
        ::std::string name;
        double probability = 0;
        ::std::vector<::tiledmap_original::Property> properties;
        int32_t tile = 0;
    };
}
namespace tiledmap_original {
    struct Grid {
        XX_OBJ_STRUCT_H(Grid)
        using IsSimpleType_v = Grid;
        int32_t height = 0;
        ::std::string orientation;
        int32_t width = 0;
    };
}
namespace tiledmap_original {
    struct Terrain {
        XX_OBJ_STRUCT_H(Terrain)
        using IsSimpleType_v = Terrain;
        ::std::string name;
        ::std::vector<::tiledmap_original::Property> properties;
        int32_t tile = 0;
    };
}
namespace tiledmap_original {
    struct TileOffset {
        XX_OBJ_STRUCT_H(TileOffset)
        using IsSimpleType_v = TileOffset;
        int32_t x = 0;
        int32_t y = 0;
    };
}
namespace tiledmap_original {
    struct Tile {
        XX_OBJ_STRUCT_H(Tile)
        using IsSimpleType_v = Tile;
        ::std::vector<::tiledmap_original::Frame> animation;
        int32_t id = 0;
        ::std::string image;
        int32_t imageheight = 0;
        int32_t imagewidth = 0;
        ::tiledmap_original::Layer objectgroup;
        double probability = 0;
        ::std::vector<::tiledmap_original::Property> properties;
        ::std::vector<int32_t> terrain;
        ::std::string type;
    };
}
namespace tiledmap_original {
    struct Transformations {
        XX_OBJ_STRUCT_H(Transformations)
        using IsSimpleType_v = Transformations;
        bool hflip = false;
        bool vflip = false;
        bool rotate = false;
        bool preferuntransformed = false;
    };
}
namespace tiledmap_original {
    struct WangSet {
        XX_OBJ_STRUCT_H(WangSet)
        using IsSimpleType_v = WangSet;
        ::std::vector<::tiledmap_original::WangColor> colors;
        ::std::string name;
        ::std::vector<::tiledmap_original::Property> properties;
        int32_t tile = 0;
        ::std::vector<::tiledmap_original::WangTile> wangtiles;
    };
}
namespace tiledmap_original {
    struct Tileset {
        XX_OBJ_STRUCT_H(Tileset)
        using IsSimpleType_v = Tileset;
        ::std::string backgroundcolor;
        int32_t columns = 0;
        int32_t firstgid = 0;
        ::tiledmap_original::Grid grid;
        ::std::string image;
        int32_t imageheight = 0;
        int32_t imagewidth = 0;
        int32_t margin = 0;
        ::std::string name;
        ::std::string objectalignment;
        ::std::vector<::tiledmap_original::Property> properties;
        ::std::string source;
        int32_t spacing = 0;
        ::std::vector<::tiledmap_original::Terrain> terrains;
        int32_t tilecount = 0;
        ::std::string tiledversion;
        int32_t tileheight = 0;
        ::tiledmap_original::TileOffset tileoffset;
        ::std::vector<::tiledmap_original::Tile> tiles;
        int32_t tilewidth = 0;
        ::tiledmap_original::Transformations transformations;
        ::std::string transparentcolor;
        ::std::string type;
        ::std::string version;
        ::std::vector<::tiledmap_original::WangSet> wangsets;
    };
}
namespace tiledmap_original {
    struct Map {
        XX_OBJ_STRUCT_H(Map)
        using IsSimpleType_v = Map;
        ::std::string backgroundcolor;
        int32_t compressionlevel = 0;
        int32_t height = 0;
        int32_t hexsidelength = 0;
        bool infinite = false;
        ::std::vector<::tiledmap_original::Layer> layers;
        int32_t nextlayerid = 0;
        int32_t nextobjectid = 0;
        ::std::string orientation;
        ::std::vector<::tiledmap_original::Property> properties;
        ::std::string renderorder;
        ::std::string staggeraxis;
        ::std::string staggerindex;
        ::std::string tiledversion;
        int32_t tileheight = 0;
        ::std::vector<::tiledmap_original::Tileset> tilesets;
        int32_t tilewidth = 0;
        ::std::string type;
        ::std::string version;
        int32_t width = 0;
    };
}
namespace xx {
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Property)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Property, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Point)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Point, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Chunk)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Chunk, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Object)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Object, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Layer)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Layer, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Frame)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Frame, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::WangTile)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::WangTile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::WangColor)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::WangColor, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Grid)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Grid, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Terrain)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Terrain, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::TileOffset)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::TileOffset, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Tile)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Transformations)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Transformations, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::WangSet)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::WangSet, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Tileset)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Tileset, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
	XX_OBJ_STRUCT_TEMPLATE_H(::tiledmap_original::Map)
    template<typename T> struct DataFuncs<T, std::enable_if_t<std::is_same_v<::tiledmap_original::Map, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) { (*(xx::ObjManager*)-1).Write(d, in); }
		static inline int Read(Data_r& d, T& out) { return (*(xx::ObjManager*)-1).Read(d, out); }
    };
}
#include <tiledmap_.h.inc>
