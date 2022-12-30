#include "pch.h"
#include "tmxdata.h"
#include "pugixml.hpp"

int TMXData::Fill(Engine* const& eg, std::string_view const& tmxfn) {
	pugi::xml_document docTmx, docTsx;
	std::string rootPath;

	// load
	if (auto&& [d, fp] = eg->ReadAllBytes(tmxfn); !d) {
		throw std::logic_error("read file error: " + std::string(tmxfn));
	} else if (auto r = docTmx.load_buffer(d.buf, d.len)) {
		throw std::logic_error("docTmx.load_buffer error: " + std::string(r.description()));
	} else {
		if (auto i = fp.find_last_of("/"); i != fp.npos) {
			rootPath = fp.substr(0, i + 1);
		}
	}

	// cleanup
	map.tilesets.clear();
	map.layers.clear();
	map.properties.clear();
	// map. objectgroup s ?

	// fill map
	auto&& cMap = docTmx.child("map");
	map.version = cMap.attribute("version").as_string();
	map.tiledversion = cMap.attribute("tiledversion").as_string();
	FillEnumTo(map.orientation, cMap.attribute("orientation").as_string());
	FillEnumTo(map.renderorder, cMap.attribute("renderorder").as_string());
	map.width = cMap.attribute("width").as_uint();
	map.height = cMap.attribute("height").as_uint();
	map.tilewidth = cMap.attribute("tilewidth").as_uint();
	map.tileheight = cMap.attribute("tileheight").as_uint();
	map.infinite = cMap.attribute("infinite").as_uint() == 1;
	map.nextlayerid = cMap.attribute("nextlayerid").as_uint();
	map.nextobjectid = cMap.attribute("nextobjectid").as_uint();

	// fill map/tileset
	auto&& map_tileset = map.tilesets.emplace_back();
	auto&& cMap_TileSet = cMap.child("tileset");
	map_tileset.firstgid = cMap_TileSet.attribute("firstgid").as_uint();
	map_tileset.source = rootPath + cMap_TileSet.attribute("source").as_string();

	if (auto&& [d, fp] = eg->ReadAllBytes(map_tileset.source); !d) {
		throw std::logic_error("read file error: " + map_tileset.source);
	} else if (auto r = docTsx.load_buffer(d.buf, d.len)) {
		throw std::logic_error("docTsx.load_buffer error: " + std::string(r.description()));
	} else {
		auto&& cTileset = docTsx.child("tileset");
		map_tileset.version = cTileset.attribute("version").as_string();
		map_tileset.tiledversion = cTileset.attribute("tiledversion").as_string();
		map_tileset.name = cTileset.attribute("name").as_string();
		map_tileset.tilewidth = cTileset.attribute("tilewidth").as_uint();
		map_tileset.tileheight = cTileset.attribute("tileheight").as_uint();
		map_tileset.spacing = cTileset.attribute("spacing").as_uint();
		map_tileset.margin = cTileset.attribute("margin").as_uint();
		map_tileset.tilecount = cTileset.attribute("tilecount").as_uint();
		map_tileset.columns = cTileset.attribute("columns").as_uint();

		auto&& cTileset_Image = cTileset.child("image");
		map_tileset.source = rootPath + cTileset_Image.attribute("source").as_string();
		map_tileset.imagewidth = cTileset_Image.attribute("width").as_uint();
		map_tileset.imageheight = cTileset_Image.attribute("height").as_uint();

		auto&& cTileset_Wangsets = cTileset.child("wangsets");
		for (auto&& cW : cTileset_Wangsets.children("wangset")) {
			auto&& w = map_tileset.wangsets.emplace_back();
			w.name = cW.attribute("name").as_string();
			FillEnumTo(w.type, cW.attribute("type").as_string());
			w.tile = cW.attribute("tile").as_uint();

			for (auto&& cC : cW.children("wangcolor")) {
				auto&& c = w.colors.emplace_back();
				c.name = cC.attribute("name").as_string();
				c.color = cC.attribute("color").as_string();
				c.tile = cC.attribute("tile").as_uint();
				c.probability = cC.attribute("probability").as_double();
			}

			for (auto&& cT : cW.children("wangtile")) {
				auto&& t = w.wangtiles.emplace_back();
				t.tileId = cT.attribute("tileid").as_uint();
				FillIntsTo(t.wangid, cT.attribute("wangid").as_string());
			}
		}
	}

	// fill map/layer
	// fill map/layer/data
	if (map.infinite) {
		// todo: data struct from array chage to  <chunk x="0" y="0" width="16" height="16"> ..... </chunk> ...
	}
	return 0;
}
