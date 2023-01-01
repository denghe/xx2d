#include "pch.h"
#include "tmxdata.h"
#include <zstd.h>

namespace TMX {

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

	// example: <... color="#fefbfcfd" AARRGGBB ...  color="fbfcfd" RRGGBB
	inline static void FillColorTo(RGBA8& out, pugi::xml_attribute const& a) {
		std::string_view sv(a.as_string());
		if (sv.size() < 6) goto LabEnd;
		if (sv[0] == '#') {
			sv = sv.substr(1);
		}
		switch (sv.size()) {
		case 6:
			out.a = 0xFFu;
			out.r = ((uint32_t)xx::FromHex(sv[0]) << 8) | (uint32_t)xx::FromHex(sv[1]);
			out.g = ((uint32_t)xx::FromHex(sv[2]) << 8) | (uint32_t)xx::FromHex(sv[3]);
			out.b = ((uint32_t)xx::FromHex(sv[4]) << 8) | (uint32_t)xx::FromHex(sv[5]);
			return;
		case 8:
			out.a = ((uint32_t)xx::FromHex(sv[0]) << 8) | (uint32_t)xx::FromHex(sv[1]);
			out.r = ((uint32_t)xx::FromHex(sv[2]) << 8) | (uint32_t)xx::FromHex(sv[3]);
			out.g = ((uint32_t)xx::FromHex(sv[4]) << 8) | (uint32_t)xx::FromHex(sv[5]);
			out.b = ((uint32_t)xx::FromHex(sv[6]) << 8) | (uint32_t)xx::FromHex(sv[7]);
			return;
		default:
			break;
		}
	LabEnd:
		throw std::logic_error("FillColorTo error: bad color string: " + std::string(sv));
	}

	// example: <polygon points="0,0 1600,448 768,-1728"/>
	inline static void FillPointsTo(std::vector<Pointi>& out, std::string_view const& sv) {
		Pointi p{};
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
			} else if (c == ' ') {
				fillX = true;
				if (negative) {
					p.y = -p.y;
					negative = false;
				}
				out.emplace_back(p);
				p = {};
			} else if (c == '-') {
				negative = true;
			} else {
				if (fillX) {
					p.x = p.x * 10 + (c - '0');
				} else {
					p.y = p.y * 10 + (c - '0');
				}
			}
		}
		if (negative) {
			p.y = -p.y;
		}
		out.emplace_back(p);
	}
	inline static void FillPointsTo(std::vector<Pointi>& out, pugi::xml_attribute const& a) {
		FillPointsTo(out, a.as_string());
	}

	template<typename T>
	inline static void FillCsvIntsTo(std::vector<T>& out, std::string_view const& csv) {
		T v{};
		for (uint8_t c : csv) {
			if (c == '\r' || c == '\n' || c == '\t' || c == ' ')
				continue;
			if (c >= '0' && c <= '9') {
				v = v * 10 + (c - '0');
			} else if (c == ',') {
				out.push_back(v);
				v = {};
			} else throw std::logic_error(std::string("FillCsvIntsTo error. unhandled c: ") + (char)c);
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
		using M = StrToEnumMappings<T>;
		for (size_t i = 0; i < M::count; ++i) {
			if (M::svs[i] == sv) {
				out = (T)i;
				return;
			}
		}
		throw std::logic_error("can't find enum by value: " + std::string(sv));
	}
	template<typename ET>
	inline static void FillEnumTo(ET& out, pugi::xml_attribute const& a) {
		FillEnumTo(out, a.as_string());
	}

	/**************************************************************************************************/

	template<typename T, typename U = std::decay_t<T>>
	inline static void TryFill(T& out, pugi::xml_attribute const& a) {
		if (a.empty()) return;
		if constexpr (std::is_same_v<U, std::string>) {
			out = a.as_string();
		} else if constexpr (std::is_same_v<U, bool>) {
			out = a.as_bool();
		} else if constexpr (std::is_same_v<U, float>) {
			out = a.as_float();
		} else if constexpr (std::is_same_v<U, double>) {
			out = a.as_double();
		} else if constexpr (std::is_same_v<U, int32_t>) {
			out = a.as_int();
		} else if constexpr (std::is_same_v<U, uint32_t>) {
			out = a.as_uint();
		} else if constexpr (std::is_same_v<U, int64_t>) {
			out = a.as_llong();
		} else if constexpr (std::is_same_v<U, uint64_t>) {
			out = a.as_ullong();
		} else if constexpr (std::is_same_v<U, RGBA8>) {
			FillColorTo(out, a);
		} else if constexpr (std::is_enum_v<U>) {
			FillEnumTo(out, a);
		} else if constexpr (xx::IsOptional_v<U>) {
			TryFill(out.emplace(), a);
		} else {
			throw std::logic_error("unhandled attribute data type");
		}
	}

	inline static void TryFillChunkBase(Chunk& chunk, pugi::xml_node const& cChunk) {
		TryFill(chunk.pos.x, cChunk.attribute("x"));
		TryFill(chunk.pos.y, cChunk.attribute("y"));
		TryFill(chunk.width, cChunk.attribute("width"));
		TryFill(chunk.height, cChunk.attribute("height"));
	}

	void Map::TryFillProperties(std::vector<xx::Shared<Property>>& out, pugi::xml_node const& owner, bool needOverride) {
		auto&& cProperties = owner.child("properties");
		for (auto&& cProperty : cProperties.children("property")) {
			PropertyTypes pt;
			FillEnumTo(pt, cProperty.attribute("type"));
			xx::Shared<Property> a;
			switch (pt) {
			case PropertyTypes::Bool: {
				auto o = xx::Make<Property_Bool>();
				o->value = cProperty.attribute("value").as_bool();
				a = o;
				break;
			}
			case PropertyTypes::Color: {
				auto o = xx::Make<Property_Color>();
				FillColorTo(o->value, cProperty.attribute("value"));
				a = o;
				break;
			}
			case PropertyTypes::Float: {
				auto o = xx::Make<Property_Float>();
				o->value = cProperty.attribute("value").as_double();
				a = o;
				break;
			}
			case PropertyTypes::File: {
				auto o = xx::Make<Property_File>();
				o->value = std::string(rootPath) + cProperty.attribute("value").as_string();
				a = o;
				break;
			}
			case PropertyTypes::Int: {
				auto o = xx::Make<Property_Int>();
				o->value = cProperty.attribute("value").as_llong();
				a = o;
				break;
			}
			case PropertyTypes::Object: {
				auto o = xx::Make<Property_Object>();
				o->objectId = cProperty.attribute("value").as_uint();
				a = o;
				break;
			}
			case PropertyTypes::String: {
				auto o = xx::Make<Property_String>();
				o->value = cProperty.attribute("value").as_string();
				a = o;
				break;
			}
			default:
				throw std::logic_error("FillPropertiesTo error: unhandled property type: " + std::to_string((int)pt));
			}
			a->type = pt;
			a->name = cProperty.attribute("name").as_string();
			if (needOverride) {
				auto iter = std::find_if(out.begin(), out.end(), [&](xx::Shared<Property> const& p) {
					return p->name == a->name;
					});
				if (iter == out.end()) {
					out.emplace_back(std::move(a));
				} else {
					*iter = std::move(a);
				}
			} else {
				out.emplace_back(std::move(a));
			}
		}
	}

	void Map::TryFillLayerBase(Layer& L, pugi::xml_node const& c) {
		TryFill(L.id, c.attribute("id"));
		TryFill(L.name, c.attribute("name"));
		TryFill(L.class_, c.attribute("class"));
		TryFill(L.visible, c.attribute("visible"));
		TryFill(L.locked, c.attribute("locked"));
		TryFill(L.opacity, c.attribute("opacity"));
		TryFill(L.tintColor, c.attribute("tintcolor"));
		TryFill(L.horizontalOffset, c.attribute("offsetx"));
		TryFill(L.verticalOffset, c.attribute("offsety"));
		TryFill(L.parallaxFactor.x, c.attribute("parallaxx"));
		TryFill(L.parallaxFactor.y, c.attribute("parallaxy"));
		TryFillProperties(L.properties, c);
	}

	void Map::TryFillLayer(Layer_Tile& L, pugi::xml_node const& c) {
		L.type = LayerTypes::Tile;
		TryFillLayerBase(L, c);
		auto&& [encoding, compression] = this->tileLayerFormat;
		auto&& cData = c.child("data");
		if (this->infinite) {
			switch (encoding) {
			case Encodings::Csv: {
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = L.chunks.emplace_back();
					TryFillChunkBase(chunk, cChunk);
					FillCsvIntsTo(chunk.gids, cChunk.text().as_string());
				}
				break;
			}
			case Encodings::Base64: {
				xx::Data tmp;
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = L.chunks.emplace_back();
					TryFillChunkBase(chunk, cChunk);
					auto bin = xx::Base64Decode(xx::Trim(cChunk.text().as_string()));
					switch (compression) {
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
						throw std::logic_error("unsupported compression: " + (int)compression);
					}
					}
				}
				break;
			}
			case Encodings::Xml: {
				for (auto&& cChunk : cData.children("chunk")) {
					auto&& chunk = L.chunks.emplace_back();
					TryFillChunkBase(chunk, cChunk);
					for (auto&& cTile : cChunk.children("tile")) {
						chunk.gids.push_back(cTile.attribute("gid").as_uint());
					}
				}
				break;
			}
			default:
				throw std::logic_error("unsupported encoding: " + (int)encoding);
			};
		} else {
			switch (this->tileLayerFormat.first) {
			case Encodings::Csv: {
				FillCsvIntsTo(L.gids, cData.text().as_string());
				break;
			}
			case Encodings::Base64: {
				xx::Data tmp;
				auto bin = xx::Base64Decode(xx::Trim(cData.text().as_string()));
				switch (this->tileLayerFormat.second) {
				case Compressions::Uncompressed: {
					FillBinIntsTo(L.gids, { bin.data(), bin.size() });
					break;
				}
				case Compressions::Zstd: {
					ZstdDecompress(bin, tmp);
					FillBinIntsTo(L.gids, tmp);
					break;
				}
				default: {
					throw std::logic_error("unsupported compression: " + (int)compression);
				}
				}
				break;
			}
			case Encodings::Xml: {
				for (auto&& cTile : cData.children("tile")) {
					L.gids.push_back(cTile.attribute("gid").as_uint());
				}
				break;
			}
			default:
				throw std::logic_error("unsupported encoding: " + (int)encoding);
			};
		}
	}

	void Map::TryFillLayer(Layer_Image& L, pugi::xml_node const& c) {
		L.type = LayerTypes::Image;
		TryFillLayerBase(L, c);
		// todo
	}

	void Map::TryFillLayer(Layer_Object& L, pugi::xml_node const& c) {
		L.type = LayerTypes::Object;
		TryFillLayerBase(L, c);
		TryFill(L.draworder, c.attribute("draworder"));

		for (auto&& cObject : c.children("object")) {
			auto&& o = L.objects.emplace_back();
			auto&& FillObj = [&](pugi::xml_node& cObj) {
				bool needOverrideProperties = o;
				if (auto&& cText = cObj.child("text"); o && o->type == ObjectTypes::Text || !cText.empty()) {
					xx::Shared<Object_Text> a;
					if (o) {
						a = o.ReinterpretCast<Object_Text>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Text;
					}
					TryFill(a->width, cObj.attribute("width"));
					TryFill(a->height, cObj.attribute("height"));
					TryFill(a->fontfamily, cObj.attribute("fontfamily"));
					TryFill(a->pixelsize, cObj.attribute("pixelsize"));
					TryFill(a->wrap, cObj.attribute("wrap"));
					TryFill(a->color, cObj.attribute("color"));
					TryFill(a->bold, cObj.attribute("bold"));
					TryFill(a->italic, cObj.attribute("italic"));
					TryFill(a->underline, cObj.attribute("underline"));
					TryFill(a->strikeout, cObj.attribute("strikeout"));
					TryFill(a->kerning, cObj.attribute("kerning"));
					TryFill(a->halign, cObj.attribute("halign"));
					TryFill(a->valign, cObj.attribute("valign"));
					if (!cText.empty()) a->text = cText.text().as_string();
					o = std::move(a);
				} else if (auto&& cPolygon = cObj.child("polygon"); o && o->type == ObjectTypes::Polygon || !cPolygon.empty()) {
					xx::Shared<Object_Polygon> a;
					if (o) {
						a = o.ReinterpretCast<Object_Polygon>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Polygon;
					}
					if (auto&& attr = cObj.attribute("points"); !attr.empty()) FillPointsTo(a->points, attr.as_string());
					o = std::move(a);
				} else if (auto&& cEllipse = cObj.child("ellipse"); o && o->type == ObjectTypes::Ellipse || !cEllipse.empty()) {
					xx::Shared<Object_Ellipse> a;
					if (o) {
						a = o.ReinterpretCast<Object_Ellipse>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Ellipse;
					}
					TryFill(a->width, cObj.attribute("width"));
					TryFill(a->height, cObj.attribute("height"));
					o = std::move(a);
				} else if (auto&& cPoint = cObj.child("point"); o && o->type == ObjectTypes::Point || !cPoint.empty()) {
					xx::Shared<Object_Point> a;
					if (o) {
						a = o.ReinterpretCast<Object_Point>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Point;
					}
					o = std::move(a);
				} else if (auto&& aGid = cObj.attribute("gid"); o && o->type == ObjectTypes::Tile || !aGid.empty()) {
					xx::Shared<Object_Tile> a;
					if (o) {
						a = o.ReinterpretCast<Object_Tile>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Tile;
					}
					TryFill(a->width, cObj.attribute("width"));
					TryFill(a->height, cObj.attribute("height"));
					if (auto&& attr = cObj.attribute("gid"); !attr.empty()) {
						a->gid = attr.as_uint();
						a->flippingHorizontal = (a->gid >> 31) & 1;
						a->flippingVertical = (a->gid >> 30) & 1;
						a->gid &= 0x3FFFFFFFu;
					}
					o = std::move(a);
				} else {
					xx::Shared<Object_Rectangle> a;
					if (o) {
						a = o.ReinterpretCast<Object_Rectangle>();
					} else {
						a.Emplace();
						a->type = ObjectTypes::Rectangle;
					}
					TryFill(a->width, cObj.attribute("width"));
					TryFill(a->height, cObj.attribute("height"));
					o = std::move(a);
				}
				if (auto&& attr = cObj.attribute("id"); !attr.empty()) o->id = attr.as_uint();
				if (auto&& attr = cObj.attribute("name"); !attr.empty()) o->name = attr.as_string();
				if (auto&& attr = cObj.attribute("class"); !attr.empty()) o->class_ = attr.as_string();
				if (auto&& attr = cObj.attribute("x"); !attr.empty()) o->x = attr.as_double();
				if (auto&& attr = cObj.attribute("y"); !attr.empty()) o->y = attr.as_double();
				if (auto&& attr = cObj.attribute("visible"); !attr.empty()) o->visible = attr.as_bool();
				if (auto&& attr = cObj.attribute("rotation"); !attr.empty()) o->rotation = attr.as_double();
				TryFillProperties(o->properties, cObj, needOverrideProperties);
			};

			if (auto&& aTemplate = cObject.attribute("template"); !aTemplate.empty()) {
				auto fn = rootPath + aTemplate.as_string();
				if (auto&& [d, fp] = eg->ReadAllBytes(fn); !d) {
					throw std::logic_error("read file error: " + fn);
				} else if (auto r = docTx->load_buffer(d.buf, d.len); r.status) {
					throw std::logic_error("docTx.load_buffer error: " + std::string(r.description()));
				} else {
					auto&& cObj = docTx->child("template").child("object");
					FillObj(cObj);
				}
			}
			FillObj(cObject);
		}
	}

	void Map::TryFillLayer(Layer_Group& L, pugi::xml_node const& c) {
		L.type = LayerTypes::Group;
		TryFillLayerBase(L, c);
		// ...
	}


	int Map::Fill(Engine* const& eg, std::string_view const& tmxfn) {
		// cleanup
		*this = {};
		this->eg = eg;
		

		// load
		if (auto&& [d, fp] = eg->ReadAllBytes(tmxfn); !d) {
			throw std::logic_error("read file error: " + std::string(tmxfn));
		} else if (auto r = docTmx->load_buffer(d.buf, d.len); r.status) {
			throw std::logic_error("docTmx.load_buffer error: " + std::string(r.description()));
		} else {
			if (auto i = fp.find_last_of("/"); i != fp.npos) {
				rootPath = fp.substr(0, i + 1);
			}
		}


		// fill map
		auto&& cMap = docTmx->child("map");
		TryFill(this->class_, cMap.attribute("class"));
		TryFill(this->orientation, cMap.attribute("orientation"));
		TryFill(this->width, cMap.attribute("width"));
		TryFill(this->height, cMap.attribute("height"));
		TryFill(this->tileWidth, cMap.attribute("tilewidth"));
		TryFill(this->tileHeight, cMap.attribute("tileheight"));
		TryFill(this->infinite, cMap.attribute("infinite"));
		TryFill(this->tileSideLength, cMap.attribute("hexsidelength"));
		TryFill(this->staggeraxis, cMap.attribute("staggeraxis"));
		TryFill(this->staggerindex, cMap.attribute("staggerindex"));
		TryFill(this->parallaxOrigin.x, cMap.attribute("parallaxoriginx"));
		TryFill(this->parallaxOrigin.y, cMap.attribute("parallaxoriginy"));
		if (auto&& cLayer = cMap.child("layer"); !cLayer.empty()) {
			if (auto&& cData = cLayer.child("data"); !cData.empty()) {
				TryFill(this->tileLayerFormat.first, cData.attribute("encoding"));
				TryFill(this->tileLayerFormat.second, cData.attribute("compression"));
			}
		}
		if (auto&& cEditorSettings = cMap.child("editorsettings"); !cEditorSettings.empty()) {
			if (auto&& cChunkSize = cEditorSettings.child("chunksize"); !cChunkSize.empty()) {
				TryFill(this->outputChunkWidth, cChunkSize.attribute("width"));
				TryFill(this->outputChunkHeight, cChunkSize.attribute("height"));
			}
		}
		TryFill(this->renderOrder, cMap.attribute("renderorder"));
		TryFill(this->compressionLevel, cMap.attribute("compressionlevel"));
		TryFill(this->backgroundColor, cMap.attribute("backgroundcolor"));
		TryFillProperties(this->properties, cMap);

		TryFill(this->version, cMap.attribute("version"));
		TryFill(this->tiledVersion, cMap.attribute("tiledversion"));
		TryFill(this->nextLayerId, cMap.attribute("nextlayerid"));
		TryFill(this->nextObjectId, cMap.attribute("nextobjectid"));

		for (auto&& c : cMap.children()) {
			std::string_view name(c.name());
			if (name == "tileset"sv) {
				auto&& ts = *this->tilesets.emplace_back().Emplace();
				TryFill(ts.firstgid, c.attribute("firstgid"));
				TryFill(ts.source, c.attribute("source"));
				ts.source = rootPath + ts.source;	// to fullpath

				if (auto&& [d, fp] = eg->ReadAllBytes(ts.source); !d) {
					throw std::logic_error("read file error: " + ts.source);
				} else if (auto r = docTsx->load_buffer(d.buf, d.len); r.status) {
					throw std::logic_error("docTsx.load_buffer error: " + std::string(r.description()));
				} else {
					auto&& cTileset = docTsx->child("tileset");
					TryFill(ts.name, cTileset.attribute("name"));
					TryFill(ts.class_, cTileset.attribute("class"));
					TryFill(ts.objectAlignment, cTileset.attribute("objectalignment"));
					if (auto&& cTileOffset = cTileset.child("tileoffset"); !cTileOffset.empty()) {
						TryFill(ts.drawingOffset.x, cTileOffset.attribute("x"));
						TryFill(ts.drawingOffset.y, cTileOffset.attribute("y"));
					}
					TryFill(ts.tileRenderSize, cTileset.attribute("tilerendersize"));
					TryFill(ts.fillMode, cTileset.attribute("fillmode"));
					TryFill(ts.backgroundColor, cTileset.attribute("backgroundcolor"));
					if (auto&& cGrid = cTileset.child("grid"); !cGrid.empty()) {
						TryFill(ts.orientation, cGrid.attribute("orientation"));
						TryFill(ts.gridWidth, cGrid.attribute("width"));
						TryFill(ts.gridHeight, cGrid.attribute("height"));
					}
					TryFill(ts.columns, cTileset.attribute("columns"));
					if (auto&& cTran = cTileset.child("transformations"); !cTran.empty()) {
						TryFill(ts.allowedTransformations.flipHorizontally, cTran.attribute("hflip"));
						TryFill(ts.allowedTransformations.flipVertically, cTran.attribute("vflip"));
						TryFill(ts.allowedTransformations.rotate, cTran.attribute("rotate"));
						TryFill(ts.allowedTransformations.preferUntransformedTiles, cTran.attribute("preferuntransformed"));
					}
					if (auto&& cImage = cTileset.child("image"); !cImage.empty()) {
						std::string s;
						TryFill(s, cImage.attribute("source"));
						s = rootPath + s;	// to fullpath
						if (s.ends_with(".png"sv)) {			// hack: replace ext to pkm
							s.resize(s.size() - 4);
							s.append(".pkm");
						}
						auto&& iter = std::find_if(this->images.begin(), this->images.end(), [&](xx::Shared<Image> const& img) {
							return img->source == s;
							});
						if (iter == this->images.end()) {
							auto&& img = *ts.image.Emplace();
							img.source = std::move(s);
							img.texture.Emplace(eg->LoadTexture(img.source));
							TryFill(img.width, cImage.attribute("width"));
							TryFill(img.height, cImage.attribute("height"));
							TryFill(img.transparentColor, cImage.attribute("trans"));
							this->images.push_back(ts.image);
						} else {
							ts.image = *iter;
						}
					}
					TryFill(ts.tilewidth, cTileset.attribute("tilewidth"));
					TryFill(ts.tileheight, cTileset.attribute("tileheight"));
					TryFill(ts.margin, cTileset.attribute("margin"));
					TryFill(ts.spacing, cTileset.attribute("spacing"));
					TryFillProperties(ts.properties, cTileset);

					TryFill(ts.version, cTileset.attribute("version"));
					TryFill(ts.tiledversion, cTileset.attribute("tiledversion"));
					TryFill(ts.tilecount, cTileset.attribute("tilecount"));

					for (auto&& cW : cTileset.child("wangsets").children("wangset")) {
						auto&& w = ts.wangSets.emplace_back();
						TryFill(w.name, cW.attribute("name"));
						TryFill(w.type, cW.attribute("type"));
						TryFill(w.tile, cW.attribute("tile"));
						TryFillProperties(w.properties, cW);

						for (auto&& cC : cW.children("wangcolor")) {
							auto&& c = w.wangColors.emplace_back();
							c.name = cC.attribute("name").as_string();
							TryFill(c.color, cC.attribute("color"));
							TryFill(c.tile, cC.attribute("tile"));
							TryFill(c.probability, cC.attribute("probability"));
							TryFillProperties(c.properties, cC);
						}

						for (auto&& cT : cW.children("wangtile")) {
							auto&& t = w.wangTiles.emplace_back();
							TryFill(t.tileId, cT.attribute("tileid"));
							FillCsvIntsTo(t.wangIds, cT.attribute("wangid").as_string());
						}
					}
				}
			} else if (name == "layer"sv) {
				auto L = xx::Make<Layer_Tile>();
				TryFillLayer(*L, c);
				this->layers.emplace_back(std::move(L));
			} else if (name == "imagelayer"sv) {
				auto L = xx::Make<Layer_Image>();
				TryFillLayer(*L, c);
				this->layers.emplace_back(std::move(L));
			} else if (name == "objectgroup"sv) {
				auto L = xx::Make<Layer_Object>();
				TryFillLayer(*L, c);
				this->layers.emplace_back(std::move(L));
			} else if (name == "group"sv) {
				auto L = xx::Make<Layer_Group>();
				TryFillLayer(*L, c);
				this->layers.emplace_back(std::move(L));
			} else {
				throw std::logic_error("unhandled layer type");
			}
		}

		// 
		//// fill map/layer...
		//for (auto&& cLayer : cMap.children("layer")) {
		//	auto&& layer = map.layers.emplace_back();
		//	layer.id = cLayer.attribute("id").as_uint();
		//	layer.name = cLayer.attribute("name").as_string();
		//	layer.width = cLayer.attribute("width").as_uint();
		//	layer.height = cLayer.attribute("height").as_uint();
		//	layer.visible = cLayer.attribute("visible").as_bool();
		//	FillColorTo(layer.tintcolor, cLayer.attribute("tintcolor"));
		//	FillColorTo(layer.transparentcolor, cLayer.attribute("transparentcolor"));
		//	FillPropertiesTo(layer.properties, cLayer, rootPath);
		//	// todo: layer type switch?

		//	auto&& cData = cLayer.child("data");
		//	FillEnumTo(layer.encoding, cData.attribute("encoding").as_string("xml"));
		//	if (layer.encoding == Encodings::Base64) {
		//		FillEnumTo(layer.compression, cData.attribute("compression").as_string("uncompressed"));
		//	} else {
		//		layer.compression = Compressions::Uncompressed;
		//	}
		//	if (map.infinite) {
		//		switch (layer.encoding) {
		//		case Encodings::Csv: {
		//			for (auto&& cChunk : cData.children("chunk")) {
		//				auto&& chunk = layer.chunks.emplace_back();
		//				chunk.x = cChunk.attribute("x").as_uint();
		//				chunk.y = cChunk.attribute("y").as_uint();
		//				chunk.width = cChunk.attribute("width").as_uint();
		//				chunk.height = cChunk.attribute("height").as_uint();
		//				FillCsvIntsTo(chunk.gids, cChunk.text().as_string());
		//			}
		//			break;
		//		}
		//		case Encodings::Base64: {
		//			xx::Data tmp;
		//			for (auto&& cChunk : cData.children("chunk")) {
		//				auto&& chunk = layer.chunks.emplace_back();
		//				chunk.x = cChunk.attribute("x").as_uint();
		//				chunk.y = cChunk.attribute("y").as_uint();
		//				chunk.width = cChunk.attribute("width").as_uint();
		//				chunk.height = cChunk.attribute("height").as_uint();
		//				auto bin = xx::Base64Decode(xx::Trim(cChunk.text().as_string()));
		//				switch (layer.compression) {
		//				case Compressions::Uncompressed: {
		//					FillBinIntsTo(chunk.gids, { bin.data(), bin.size() });
		//					break;
		//				}
		//				case Compressions::Zstd: {
		//					ZstdDecompress(bin, tmp);
		//					FillBinIntsTo(chunk.gids, tmp);
		//					break;
		//				}
		//				default: {
		//					throw std::logic_error("unsupported compression: " + (int)layer.compression);
		//				}
		//				}
		//			}
		//			break;
		//		}
		//		case Encodings::Xml: {
		//			for (auto&& cChunk : cData.children("chunk")) {
		//				auto&& chunk = layer.chunks.emplace_back();
		//				chunk.x = cChunk.attribute("x").as_uint();
		//				chunk.y = cChunk.attribute("y").as_uint();
		//				chunk.width = cChunk.attribute("width").as_uint();
		//				chunk.height = cChunk.attribute("height").as_uint();
		//				for (auto&& cTile : cChunk.children("tile")) {
		//					chunk.gids.push_back(cTile.attribute("gid").as_uint());
		//				}
		//			}
		//			break;
		//		}
		//		default:
		//			throw std::logic_error("unsupported encoding: " + (int)layer.encoding);
		//		};
		//	} else {
		//		switch (layer.encoding) {
		//		case Encodings::Csv: {
		//			FillCsvIntsTo(layer.gids, cData.text().as_string());
		//			break;
		//		}
		//		case Encodings::Base64: {
		//			xx::Data tmp;
		//			auto bin = xx::Base64Decode(xx::Trim(cData.text().as_string()));
		//			switch (layer.compression) {
		//			case Compressions::Uncompressed: {
		//				FillBinIntsTo(layer.gids, { bin.data(), bin.size() });
		//				break;
		//			}
		//			case Compressions::Zstd: {
		//				ZstdDecompress(bin, tmp);
		//				FillBinIntsTo(layer.gids, tmp);
		//				break;
		//			}
		//			default: {
		//				throw std::logic_error("unsupported compression: " + (int)layer.compression);
		//			}
		//			}
		//			break;
		//		}
		//		case Encodings::Xml: {
		//			for (auto&& cTile : cData.children("tile")) {
		//				layer.gids.push_back(cTile.attribute("gid").as_uint());
		//			}
		//			break;
		//		}
		//		default:
		//			throw std::logic_error("unsupported encoding: " + (int)layer.encoding);
		//		};
		//	}
		//}

		//// fill map/objectgroup...
		//for (auto&& cObjectgroup : cMap.children("objectgroup")) {
		//	auto&& og = map.objectgroups.emplace_back();
		//	og.id = cObjectgroup.attribute("id").as_uint();
		//	og.name = cObjectgroup.attribute("name").as_string();
		//	og.class_ = cObjectgroup.attribute("class").as_string();
		//	og.locked = cObjectgroup.attribute("locked").as_bool();
		//	og.opacity = cObjectgroup.attribute("opacity").as_double();
		//	og.visible = cObjectgroup.attribute("visible").as_bool();
		//	FillEnumTo(og.draworder, cObjectgroup.attribute("draworder").as_string("topdown"));
		//	og.offsetx = cObjectgroup.attribute("offsetx").as_double();
		//	og.offsety = cObjectgroup.attribute("offsety").as_double();
		//	og.parallaxx = cObjectgroup.attribute("parallaxx").as_double();
		//	og.parallaxy = cObjectgroup.attribute("parallaxy").as_double();
		//	FillColorTo(og.tintcolor, cObjectgroup.attribute("tintcolor"));

		//	for (auto&& cObject : cObjectgroup.children("object")) {
		//		auto&& o = og.objects.emplace_back();
		//		auto&& FillObj = [&](pugi::xml_node& cObj) {
		//			bool needOverrideProperties = o;
		//			if (auto&& cText = cObj.child("text"); o && o->type == ObjectTypes::Text || !cText.empty()) {
		//				xx::Shared<ObjectText> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectText>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Text;
		//				}
		//				if (auto&& attr = cObj.attribute("width"); !attr.empty()) a->width = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("height"); !attr.empty()) a->height = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("fontfamily"); !attr.empty()) a->fontfamily = attr.as_string();
		//				if (auto&& attr = cObj.attribute("pixelsize"); !attr.empty()) a->pixelsize = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("wrap"); !attr.empty()) a->wrap = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("color"); !attr.empty()) FillColorTo(a->color, attr.as_string());
		//				if (auto&& attr = cObj.attribute("bold"); !attr.empty()) a->bold = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("italic"); !attr.empty()) a->italic = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("underline"); !attr.empty()) a->underline = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("strikeout"); !attr.empty()) a->strikeout = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("kerning"); !attr.empty()) a->kerning = attr.as_bool();
		//				if (auto&& attr = cObj.attribute("halign"); !attr.empty()) FillEnumTo(a->halign, attr);
		//				if (auto&& attr = cObj.attribute("valign"); !attr.empty()) FillEnumTo(a->valign, attr);
		//				if (!cText.empty()) a->text = cText.text().as_string();
		//				o = std::move(a);
		//			} else if (auto&& cPolygon = cObj.child("polygon"); o && o->type == ObjectTypes::Polygon || !cPolygon.empty()) {
		//				xx::Shared<ObjectPolygon> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectPolygon>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Polygon;
		//				}
		//				if (auto&& attr = cObj.attribute("points"); !attr.empty()) FillPointsTo(a->points, attr.as_string());
		//				o = std::move(a);
		//			} else if (auto&& cEllipse = cObj.child("ellipse"); o && o->type == ObjectTypes::Ellipse || !cEllipse.empty()) {
		//				xx::Shared<ObjectEllipse> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectEllipse>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Ellipse;
		//				}
		//				if (auto&& attr = cObj.attribute("width"); !attr.empty()) a->width = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("height"); !attr.empty()) a->height = attr.as_uint();
		//				o = std::move(a);
		//			} else if (auto&& cPoint = cObj.child("point"); o && o->type == ObjectTypes::Point || !cPoint.empty()) {
		//				xx::Shared<ObjectPoint> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectPoint>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Point;
		//				}
		//				o = std::move(a);
		//			} else if (auto&& aGid = cObj.attribute("gid"); o && o->type == ObjectTypes::Tile || !aGid.empty()) {
		//				xx::Shared<ObjectTile> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectTile>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Tile;
		//				}
		//				if (auto&& attr = cObj.attribute("width"); !attr.empty()) a->width = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("height"); !attr.empty()) a->height = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("gid"); !attr.empty()) {
		//					a->gid = attr.as_uint();
		//					a->flippingHorizontal = (a->gid >> 31) & 1;
		//					a->flippingVertical = (a->gid >> 30) & 1;
		//					a->gid &= 0x3FFFFFFFu;
		//				}
		//				o = std::move(a);
		//			} else {
		//				xx::Shared<ObjectRectangle> a;
		//				if (o) {
		//					a = o.ReinterpretCast<ObjectRectangle>();
		//				} else {
		//					a.Emplace();
		//					a->type = ObjectTypes::Rectangle;
		//				}
		//				if (auto&& attr = cObj.attribute("width"); !attr.empty()) a->width = attr.as_uint();
		//				if (auto&& attr = cObj.attribute("height"); !attr.empty()) a->height = attr.as_uint();
		//				o = std::move(a);
		//			}
		//			if (auto&& attr = cObj.attribute("id"); !attr.empty()) o->id = attr.as_uint();
		//			if (auto&& attr = cObj.attribute("name"); !attr.empty()) o->name = attr.as_string();
		//			if (auto&& attr = cObj.attribute("class"); !attr.empty()) o->class_ = attr.as_string();
		//			if (auto&& attr = cObj.attribute("x"); !attr.empty()) o->x = attr.as_double();
		//			if (auto&& attr = cObj.attribute("y"); !attr.empty()) o->y = attr.as_double();
		//			if (auto&& attr = cObj.attribute("visible"); !attr.empty()) o->visible = attr.as_bool();
		//			if (auto&& attr = cObj.attribute("rotation"); !attr.empty()) o->rotation = attr.as_double();
		//			FillPropertiesTo(o->properties, cObj, rootPath, needOverrideProperties);
		//		};

		//		if (auto&& aTemplate = cObject.attribute("template"); !aTemplate.empty()) {
		//			auto fn = rootPath + aTemplate.as_string();
		//			if (auto&& [d, fp] = eg->ReadAllBytes(fn); !d) {
		//				throw std::logic_error("read file error: " + fn);
		//			} else if (auto r = docTx.load_buffer(d.buf, d.len); r.status) {
		//				throw std::logic_error("docTx.load_buffer error: " + std::string(r.description()));
		//			} else {
		//				auto&& cObj = docTx.child("template").child("object");
		//				FillObj(cObj);
		//			}
		//		}
		//		FillObj(cObject);
		//	}
		//}

		return 0;
	}


	//auto&& cTileset = cTemplate.child("tileset");
	//auto source = rootPath + cTileset.attribute("source").as_string();
	//auto&& iter = std::find_if(map.tilesets.begin(), map.tilesets.end(), [&](xx::Shared<Tileset> const& item) { return item->source == source; });
	//if (iter == map.tilesets.end()) {
	//	throw std::logic_error("object.template.tileset.source can't find in map.tilesets: " + source);
	//}

}
