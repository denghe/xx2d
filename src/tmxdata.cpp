#include "pch.h"
#include "tmxdata.h"
#include "pugixml.hpp"
#include <zstd.h>

// example: <... color="#fefbfcfd" AARRGGBB ...  color="#fbfcfd" RRGGBB
inline static void FillColorTo(RGBA8& out, std::string_view const& sv) {
	if (sv.size() < 7 || sv[0] != '#') goto LabEnd;
	switch (sv.size()) {
	case 7:
		out.a = 0xFFu;
		out.r = ((uint32_t)xx::FromHex(sv[1]) << 8) | (uint32_t)xx::FromHex(sv[2]);
		out.g = ((uint32_t)xx::FromHex(sv[3]) << 8) | (uint32_t)xx::FromHex(sv[4]);
		out.b = ((uint32_t)xx::FromHex(sv[5]) << 8) | (uint32_t)xx::FromHex(sv[6]);
		return;
	case 9:
		out.a = ((uint32_t)xx::FromHex(sv[1]) << 8) | (uint32_t)xx::FromHex(sv[2]);
		out.r = ((uint32_t)xx::FromHex(sv[3]) << 8) | (uint32_t)xx::FromHex(sv[4]);
		out.g = ((uint32_t)xx::FromHex(sv[5]) << 8) | (uint32_t)xx::FromHex(sv[6]);
		out.b = ((uint32_t)xx::FromHex(sv[7]) << 8) | (uint32_t)xx::FromHex(sv[8]);
		return;
	default:
		break;
	}
LabEnd:
	throw std::logic_error("FillColorTo error: bad color string: " + std::string(sv));
}

inline static void FillColorTo(std::optional<RGBA8>& out, pugi::xml_attribute const& a) {
	if (a.empty()) {
		out.reset();
	}
	else {
		RGBA8 c;
		FillColorTo(c, a.as_string());
		out = c;
	}
}

// example: <polygon points="0,0 1600,448 768,-1728"/>
inline static void FillPointsTo(std::vector<TMXData::Point>& out, std::string_view const& sv) {
	TMXData::Point p{};
	bool fillX = true;
	bool negative = false;
	for (uint8_t c : sv) {
		if (c == ',') {
			fillX = false;
			if (negative) {
				p.x = -p.x;
				negative = false;
			}
			continue;
		}
		else if (c == ' ') {
			fillX = true;
			if (negative) {
				p.y = -p.y;
				negative = false;
			}
			out.emplace_back(p);
			p = {};
		}
		else if (c == '-') {
			negative = true;
		}
		else {
			if (fillX) {
				p.x = p.x * 10 + (c - '0');
			}
			else {
				p.y = p.y * 10 + (c - '0');
			}
		}
	}
	if (negative) {
		p.y = -p.y;
	}
	out.emplace_back(p);
}


template<typename T>
inline static void FillCsvIntsTo(std::vector<T>& out, std::string_view const& csv) {
	T v{};
	for(uint8_t c : csv) {
		if (c == '\r' || c == '\n' || c == '\t' || c == ' ')
			continue;
		if (c >= '0' && c <= '9') {
			v = v * 10 + (c - '0');
		}
		else if (c == ',') {
			out.push_back(v);
			v = {};
		}
		else throw std::logic_error(std::string("FillCsvIntsTo error. unhandled c: ") + (char)c);
	}
	out.push_back(v);
}

template<typename T = uint32_t>
inline static void FillBinIntsTo(std::vector<T>& out, xx::Data_r dr) {
	T v;
	while (dr.HasLeft()) {
		if (int r = dr.ReadFixed(v)) throw std::logic_error("read int error. r = " + std::to_string(r));
		out.push_back(v);
	}
}

template<typename ET>
inline static void FillEnumTo(ET& out, std::string_view const& sv) {
	using T = std::decay_t<ET>;
	using M = TMXData::StrToEnumMappings<T>;
	for (size_t i = 0; i < M::count; ++i) {
		if (M::svs[i] == sv) {
			out = (T)i;
			return;
		}
	}
	throw std::logic_error("can't find enum by value: " + std::string(sv));
}


inline static void FillPropertiesTo(std::vector<xx::Shared<TMXData::Property>>& out, pugi::xml_node& owner, std::string_view const& rootPath) {
	auto&& cProperties = owner.child("properties");
	for (auto&& cProperty : cProperties.children("property")) {
		TMXData::PropertyTypes pt;
		FillEnumTo(pt, cProperty.attribute("type").as_string("string"));
		xx::Shared<TMXData::Property> a;
		switch (pt) {
		case TMXData::PropertyTypes::Bool: {
			auto o = xx::Make<TMXData::PropertyBool>();
			o->value = cProperty.attribute("value").as_bool();
			a = o;
			break;
		}
		case TMXData::PropertyTypes::Color: {
			auto o = xx::Make<TMXData::PropertyColor>();
			FillColorTo(o->value, cProperty.attribute("value").as_string());
			a = o;
			break;
		}
		case TMXData::PropertyTypes::Float: {
			auto o = xx::Make<TMXData::PropertyFloat>();
			o->value = cProperty.attribute("value").as_double();
			a = o;
			break;
		}
		case TMXData::PropertyTypes::File: {
			auto o = xx::Make<TMXData::PropertyFile>();
			o->value = std::string(rootPath) + cProperty.attribute("value").as_string();
			a = o;
			break;
		}
		case TMXData::PropertyTypes::Int: {
			auto o = xx::Make<TMXData::PropertyInt>();
			o->value = cProperty.attribute("value").as_llong();
			a = o;
			break;
		}
		case TMXData::PropertyTypes::Object: {
			auto o = xx::Make<TMXData::PropertyObject>();
			o->objectId = cProperty.attribute("value").as_uint();
			a = o;
			break;
		}
		case TMXData::PropertyTypes::String: {
			auto o = xx::Make<TMXData::PropertyString>();
			o->value = cProperty.attribute("value").as_string();
			a = o;
			break;
		}
		default:
			throw std::logic_error("FillPropertiesTo error: unhandled property type: " + std::to_string((int)pt));
		}
		a->type = pt;
		a->name = cProperty.attribute("name").as_string();
		out.emplace_back(std::move(a));
	}
}


inline void ZstdDecompress(std::string_view const& src, xx::Data& dst) {
	auto siz = ZSTD_getFrameContentSize(src.data(), src.size());
	if (ZSTD_CONTENTSIZE_UNKNOWN == siz) throw std::logic_error("ZstdDecompress error: unknown content size.");
	if (ZSTD_CONTENTSIZE_ERROR == siz) throw std::logic_error("ZstdDecompress read content size error."); 
	dst.Resize(siz);
	if (0 == siz) return;
	siz = ZSTD_decompress(dst.buf, siz, src.data(), src.size());
	if (ZSTD_isError(siz)) throw std::logic_error("ZstdDecompress decompress error.");
	dst.Resize(siz);
}


int TMXData::Fill(Engine* const& eg, std::string_view const& tmxfn) {
	pugi::xml_document docTmx, docTsx;
	std::string rootPath;

	// load
	if (auto&& [d, fp] = eg->ReadAllBytes(tmxfn); !d) {
		throw std::logic_error("read file error: " + std::string(tmxfn));
	}
	else if (auto r = docTmx.load_buffer(d.buf, d.len); r.status) {
		throw std::logic_error("docTmx.load_buffer error: " + std::string(r.description()));
	}
	else {
		if (auto i = fp.find_last_of("/"); i != fp.npos) {
			rootPath = fp.substr(0, i + 1);
		}
	}

	// cleanup
	map.tilesets.clear();
	map.layers.clear();
	map.properties.clear();
	map.objectgroups.clear();

	// fill map
	auto&& cMap = docTmx.child("map");
	map.version = cMap.attribute("version").as_string();
	map.tiledversion = cMap.attribute("tiledversion").as_string();
	FillEnumTo(map.orientation, cMap.attribute("orientation").as_string("orthogonal"));
	FillEnumTo(map.renderorder, cMap.attribute("renderorder").as_string("right-down"));
	map.width = cMap.attribute("width").as_uint();
	map.height = cMap.attribute("height").as_uint();
	map.tilewidth = cMap.attribute("tilewidth").as_uint();
	map.tileheight = cMap.attribute("tileheight").as_uint();
	map.infinite = cMap.attribute("infinite").as_bool();
	map.nextlayerid = cMap.attribute("nextlayerid").as_uint();
	map.nextobjectid = cMap.attribute("nextobjectid").as_uint();
	map.parallaxoriginx = cMap.attribute("parallaxoriginx").as_double();
	map.parallaxoriginy = cMap.attribute("parallaxoriginy").as_double();
	map.class_ = cMap.attribute("class").as_string();
	FillPropertiesTo(map.properties, cMap, rootPath);
	switch (map.orientation) {
	case Orientations::Hexagonal:
		map.hexsidelength = cMap.attribute("hexsidelength").as_uint();
	case Orientations::Staggered:
		FillEnumTo(map.staggeraxis, cMap.attribute("staggeraxis").as_string("y"));
		FillEnumTo(map.staggerindex, cMap.attribute("staggerindex").as_string("odd"));
		break;
	default:
		map.hexsidelength = 0;
		map.staggeraxis = StaggerAxiss::Y;
		map.staggerindex = StaggerIndexs::Odd;
	}
	if (auto&& cEditorSettings = cMap.child("editorsettings"); !cEditorSettings.empty()) {
		auto&& cChunkSize = cEditorSettings.child("chunksize");
		map.outputChunkWidth = cChunkSize.attribute("width").as_uint(16);
		map.outputChunkHeight = cChunkSize.attribute("height").as_uint(16);
	}
	else {
		map.outputChunkWidth = 16;
		map.outputChunkHeight = 16;
	}
	map.compressionlevel = cMap.attribute("compressionlevel").as_int(-1);
	FillColorTo(map.backgroundcolor, cMap.attribute("backgroundcolor"));

	// fill map/tileset...
	for (auto&& cTileset0 : cMap.children("tileset")) {
		auto&& ts = map.tilesets.emplace_back();
		ts.firstgid = cTileset0.attribute("firstgid").as_uint();
		ts.source = rootPath + cTileset0.attribute("source").as_string();
		FillPropertiesTo(ts.properties, cTileset0, rootPath);

		if (auto&& [d, fp] = eg->ReadAllBytes(ts.source); !d) {
			throw std::logic_error("read file error: " + ts.source);
		}
		else if (auto r = docTsx.load_buffer(d.buf, d.len); r.status) {
			throw std::logic_error("docTsx.load_buffer error: " + std::string(r.description()));
		}
		else {
			auto&& cTileset = docTsx.child("tileset");
			ts.version = cTileset.attribute("version").as_string();
			ts.tiledversion = cTileset.attribute("tiledversion").as_string();
			ts.name = cTileset.attribute("name").as_string();
			ts.class_ = cTileset.attribute("class").as_string();
			ts.tilewidth = cTileset.attribute("tilewidth").as_uint();
			ts.tileheight = cTileset.attribute("tileheight").as_uint();
			ts.spacing = cTileset.attribute("spacing").as_uint();
			ts.margin = cTileset.attribute("margin").as_uint();
			ts.tilecount = cTileset.attribute("tilecount").as_uint();
			ts.columns = cTileset.attribute("columns").as_uint();
			FillColorTo(ts.backgroundcolor, cTileset.attribute("backgroundcolor"));
			FillColorTo(ts.transparentcolor, cTileset.attribute("transparentcolor"));

			auto&& cTileset_Image = cTileset.child("image");
			ts.source = rootPath + cTileset_Image.attribute("source").as_string();
			ts.image = ts.source;
			ts.imagewidth = cTileset_Image.attribute("width").as_uint();
			ts.imageheight = cTileset_Image.attribute("height").as_uint();

			auto&& cTileset_Wangsets = cTileset.child("wangsets");
			for (auto&& cW : cTileset_Wangsets.children("wangset")) {
				auto&& w = ts.wangsets.emplace_back();
				w.name = cW.attribute("name").as_string();
				FillEnumTo(w.type, cW.attribute("type").as_string());
				w.tile = cW.attribute("tile").as_uint();
				FillPropertiesTo(w.properties, cW, rootPath);

				for (auto&& cC : cW.children("wangcolor")) {
					auto&& c = w.colors.emplace_back();
					c.name = cC.attribute("name").as_string();
					FillColorTo(c.color, cC.attribute("color").as_string());
					c.tile = cC.attribute("tile").as_uint();
					c.probability = cC.attribute("probability").as_double();
					FillPropertiesTo(c.properties, cC, rootPath);
				}

				for (auto&& cT : cW.children("wangtile")) {
					auto&& t = w.wangtiles.emplace_back();
					t.tileId = cT.attribute("tileid").as_uint();
					FillCsvIntsTo(t.wangid, cT.attribute("wangid").as_string());
				}
			}
		}
	}

	// fill map/layer...
	for (auto&& cLayer : cMap.children("layer")) {
		auto&& layer = map.layers.emplace_back();
		layer.id = cLayer.attribute("id").as_uint();
		layer.name = cLayer.attribute("name").as_string();
		layer.width = cLayer.attribute("width").as_uint();
		layer.height = cLayer.attribute("height").as_uint();
		layer.visible = cLayer.attribute("visible").as_bool();
		FillColorTo(layer.tintcolor, cLayer.attribute("tintcolor"));
		FillColorTo(layer.transparentcolor, cLayer.attribute("transparentcolor"));
		FillPropertiesTo(layer.properties, cLayer, rootPath);
		// todo: layer type switch?

		auto&& cData = cLayer.child("data");
		FillEnumTo(layer.encoding, cData.attribute("encoding").as_string("xml"));
		if (layer.encoding == Encodings::Base64) {
			FillEnumTo(layer.compression, cData.attribute("compression").as_string("uncompressed"));
		}
		else {
			layer.compression = Compressions::Uncompressed;
		}
		if (map.infinite) {
			switch (layer.encoding) {
			case Encodings::Csv: {
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = layer.chunks.emplace_back();
					chunk.x = cChunk.attribute("x").as_uint();
					chunk.y = cChunk.attribute("y").as_uint();
					chunk.width = cChunk.attribute("width").as_uint();
					chunk.height = cChunk.attribute("height").as_uint();
					FillCsvIntsTo(chunk.gids, cChunk.text().as_string());
				}
				break;
			}
			case Encodings::Base64: {
				xx::Data tmp;
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = layer.chunks.emplace_back();
					chunk.x = cChunk.attribute("x").as_uint();
					chunk.y = cChunk.attribute("y").as_uint();
					chunk.width = cChunk.attribute("width").as_uint();
					chunk.height = cChunk.attribute("height").as_uint();
					auto bin = xx::Base64Decode(xx::Trim(cChunk.text().as_string()));
					switch (layer.compression) {
					case Compressions::Uncompressed: {
						FillBinIntsTo(chunk.gids, { bin.data(), bin.size() });
						break;
					}
					case Compressions::Zstd: {
						ZstdDecompress(bin, tmp);
						FillBinIntsTo(chunk.gids, tmp);
						break;
					}
					default: {
						throw std::logic_error("unsupported compression: " + (int)layer.compression);
					}
					}
				}
				break;
			}
			case Encodings::Xml: {
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = layer.chunks.emplace_back();
					chunk.x = cChunk.attribute("x").as_uint();
					chunk.y = cChunk.attribute("y").as_uint();
					chunk.width = cChunk.attribute("width").as_uint();
					chunk.height = cChunk.attribute("height").as_uint();
					for (auto&& cTile : cChunk.children("tile")) {
						chunk.gids.push_back(cTile.attribute("gid").as_uint());
					}
				}
				break;
			}
			default:
				throw std::logic_error("unsupported encoding: " + (int)layer.encoding);
			};
		}
		else {
			switch (layer.encoding) {
			case Encodings::Csv: {
				FillCsvIntsTo(layer.gids, cData.text().as_string());
				break;
			}
			case Encodings::Base64: {
				xx::Data tmp;
				auto bin = xx::Base64Decode(xx::Trim(cData.text().as_string()));
				switch (layer.compression) {
				case Compressions::Uncompressed: {
					FillBinIntsTo(layer.gids, { bin.data(), bin.size() });
					break;
				}
				case Compressions::Zstd: {
					ZstdDecompress(bin, tmp);
					FillBinIntsTo(layer.gids, tmp);
					break;
				}
				default: {
					throw std::logic_error("unsupported compression: " + (int)layer.compression);
				}
				}
				break;
			}
			case Encodings::Xml: {
				for (auto&& cTile : cData.children("tile")) {
					layer.gids.push_back(cTile.attribute("gid").as_uint());
				}
				break;
			}
			default:
				throw std::logic_error("unsupported encoding: " + (int)layer.encoding);
			};
		}
	}

	// fill map/objectgroup...
	for (auto&& cObjectgroup : cMap.children("objectgroup")) {
		auto&& og = map.objectgroups.emplace_back();
		og.id = cObjectgroup.attribute("id").as_uint();
		og.name = cObjectgroup.attribute("name").as_string();
		og.class_ = cObjectgroup.attribute("class").as_string();
		og.locked = cObjectgroup.attribute("locked").as_bool();
		og.opacity = cObjectgroup.attribute("opacity").as_double();
		og.visible = cObjectgroup.attribute("visible").as_bool();
		FillEnumTo(og.draworder, cObjectgroup.attribute("draworder").as_string("topdown"));
		og.offsetx = cObjectgroup.attribute("offsetx").as_double();
		og.offsety = cObjectgroup.attribute("offsety").as_double();
		og.parallaxx = cObjectgroup.attribute("parallaxx").as_double();
		og.parallaxy = cObjectgroup.attribute("parallaxy").as_double();
		FillColorTo(og.tintcolor, cObjectgroup.attribute("tintcolor"));

		for (auto&& cObject : cObjectgroup.children("object")) {
			auto&& o = og.objects.emplace_back();
			if (!cObject.child("point").empty()) {
				o = xx::Make<ObjectPoint>();
			}
			else if (!cObject.child("ellipse").empty()) {
				auto a = xx::Make<ObjectEllipse>();
				a->width = cObject.attribute("width").as_uint();
				a->height = cObject.attribute("height").as_uint();
				o = std::move(a);
			}
			else if (auto&& cPolygon = cObject.child("polygon"); !cPolygon.empty()) {
				auto a = xx::Make<ObjectPolygon>();
				FillPointsTo(a->points, cPolygon.attribute("points").as_string());
				o = std::move(a);
			}
			else if (auto&& cText = cObject.child("text"); !cText.empty()) {
				auto a = xx::Make<ObjectText>();
				a->width = cObject.attribute("width").as_uint();
				a->height = cObject.attribute("height").as_uint();
				a->fontfamily = cText.attribute("fontfamily").as_string();
				a->pixelsize = cText.attribute("pixelsize").as_uint(16);
				a->wrap = cText.attribute("wrap").as_bool();
				FillColorTo(a->color, cText.attribute("color").as_string("#000000"));
				a->bold = cText.attribute("bold").as_bool();
				a->italic = cText.attribute("italic").as_bool();
				a->underline = cText.attribute("underline").as_bool();
				a->strikeout = cText.attribute("strikeout").as_bool();
				a->kerning = cText.attribute("kerning").as_bool(true);
				FillEnumTo(a->halign, cText.attribute("halign").as_string("left"));
				FillEnumTo(a->valign, cText.attribute("valign").as_string("top"));
				a->text = cText.text().as_string();
				o = std::move(a);
			}
			else if (pugi::xml_attribute w = cObject.attribute("width"), h = cObject.attribute("height"); !w.empty() && !h.empty()) {
				if (auto&& gid = cObject.attribute("gid"); gid.empty()) {
					auto a = xx::Make<ObjectRectangle>();
					a->width = w.as_uint();
					a->height = h.as_uint();
					o = std::move(a);
				}
				else {
					auto a = xx::Make<ObjectTile>();
					a->width = w.as_uint();
					a->height = h.as_uint();
					a->gid = gid.as_uint();
					a->flippingHorizontal = (a->gid >> 31) & 1;
					a->flippingVertical = (a->gid >> 30) & 1;
					a->gid &= 0x3FFFFFFFu;
					o = std::move(a);
				}
			}
			else {
				throw std::logic_error("todo: fill Object: template");
			}
			o->id = cObject.attribute("id").as_uint();
			o->name = cObject.attribute("name").as_string();
			o->class_ = cObject.attribute("class").as_string();
			o->x = cObject.attribute("x").as_double();
			o->y = cObject.attribute("y").as_double();
			o->visible = cObject.attribute("visible").as_bool();
			o->rotation = cObject.attribute("rotation").as_double(0);
			FillPropertiesTo(o->properties, cObject, rootPath);
		}
	}

	// todo: fill ptrs, textures

	return 0;
}
