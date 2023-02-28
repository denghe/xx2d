#include "pch.h"
#include "pugixml.hpp"

namespace xx {

	namespace TMX {

		struct Filler {
			// fill data by .tmx file
			Filler(Map& map, std::string_view tmxfn);

			int rtv = 0;
			Map& map;
			pugi::xml_document docTmx, docTsx, docTx;
			std::string rootPath;
			std::unordered_map<uint32_t, Object*> objs;	// store all objs cross Layer_Object
			std::vector<std::pair<std::vector<Property>*, size_t>> objProps;	// store properties[ idx ] need replace id to obj

			// for easy Fill
			void TryFillProperties(std::vector<Property>& out, pugi::xml_node const& owner, bool needOverride = false);
			void TryFillImage(xx::Shared<Image>& out, pugi::xml_node const& c);
			Property* MakeProperty(std::vector<Property>& out, pugi::xml_node const& c, std::string&& name);
			void TryFillTileset(Tileset& ts, pugi::xml_node const& c);
			void TryFillLayerBase(Layer& L, pugi::xml_node const& c);
			void TryFillLayer(Layer_Tile& L, pugi::xml_node const& c);
			void TryFillLayer(Layer_Image& L, pugi::xml_node const& c);
			void TryFillObject(xx::Shared<Object>& o, pugi::xml_node const& c);
			void TryFillLayer(Layer_Object& L, pugi::xml_node const& c);
			void TryFillLayer(Layer_Group& L, pugi::xml_node const& c);
		};


		template<typename ET> struct StrToEnumMappings {
			inline static constexpr std::string_view const* svs = nullptr;
			inline static constexpr size_t count = 0;
		};

		/**********************************************************************************/

		inline static constexpr std::array<std::string_view, 4> enumTexts_Orientations = {
			"orthogonal"sv,
			"isometric"sv,
			"staggered"sv,
			"hexagonal"sv
		};
		template<> struct StrToEnumMappings<Orientations> {
			inline static constexpr std::string_view const* svs = enumTexts_Orientations.data();
			inline static constexpr size_t count = enumTexts_Orientations.size();
		};

		inline static constexpr std::array<std::string_view, 4> enumTexts_RenderOrders = {
			"right-down"sv,
			"right-up"sv,
			"left-down"sv,
			"left-up"sv
		};
		template<> struct StrToEnumMappings<RenderOrders> {
			inline static constexpr std::string_view const* svs = enumTexts_RenderOrders.data();
			inline static constexpr size_t count = enumTexts_RenderOrders.size();
		};
		inline static constexpr std::array<std::string_view, 3> enumTexts_WangSetTypes = {
			"corner"sv,
			"edge"sv,
			"mixed"sv,
		};
		template<> struct StrToEnumMappings<WangSetTypes> {
			inline static constexpr std::string_view const* svs = enumTexts_WangSetTypes.data();
			inline static constexpr size_t count = enumTexts_WangSetTypes.size();
		};
		inline static constexpr std::array<std::string_view, 3> enumTexts_Encodings = {
			"csv"sv,
			"base64"sv,
			"xml"sv,
		};
		template<> struct StrToEnumMappings<Encodings> {
			inline static constexpr std::string_view const* svs = enumTexts_Encodings.data();
			inline static constexpr size_t count = enumTexts_Encodings.size();
		};
		inline static constexpr std::array<std::string_view, 4> enumTexts_Compressions = {
			"uncompressed"sv,
			"gzip"sv,
			"zlib"sv,
			"zstd"sv,
		};
		template<> struct StrToEnumMappings<Compressions> {
			inline static constexpr std::string_view const* svs = enumTexts_Compressions.data();
			inline static constexpr size_t count = enumTexts_Compressions.size();
		};
		inline static constexpr std::array<std::string_view, 4> enumTexts_HAligns = {
			"left"sv,
			"center"sv,
			"right"sv,
			"justify"sv,
		};
		template<> struct StrToEnumMappings<HAligns> {
			inline static constexpr std::string_view const* svs = enumTexts_HAligns.data();
			inline static constexpr size_t count = enumTexts_HAligns.size();
		};
		inline static constexpr std::array<std::string_view, 2> enumTexts_StaggerAxiss = {
			"x"sv,
			"y"sv,
		};
		template<> struct StrToEnumMappings<StaggerAxiss> {
			inline static constexpr std::string_view const* svs = enumTexts_StaggerAxiss.data();
			inline static constexpr size_t count = enumTexts_StaggerAxiss.size();
		};
		inline static constexpr std::array<std::string_view, 2> enumTexts_StaggerIndexs = {
			"odd"sv,
			"even"sv,
		};
		template<> struct StrToEnumMappings<StaggerIndexs> {
			inline static constexpr std::string_view const* svs = enumTexts_StaggerIndexs.data();
			inline static constexpr size_t count = enumTexts_StaggerIndexs.size();
		};
		inline static constexpr std::array<std::string_view, 3> enumTexts_VAligns = {
			"top"sv,
			"center"sv,
			"bottom"sv,
		};
		template<> struct StrToEnumMappings<VAligns> {
			inline static constexpr std::string_view const* svs = enumTexts_VAligns.data();
			inline static constexpr size_t count = enumTexts_VAligns.size();
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
		template<> struct StrToEnumMappings<PropertyTypes> {
			inline static constexpr std::string_view const* svs = enumTexts_PropertyTypes.data();
			inline static constexpr size_t count = enumTexts_PropertyTypes.size();
		};
		inline static constexpr std::array<std::string_view, 2> enumTexts_DrawOrders = {
			"topdown"sv,
			"index"sv,
		};
		template<> struct StrToEnumMappings<DrawOrders> {
			inline static constexpr std::string_view const* svs = enumTexts_DrawOrders.data();
			inline static constexpr size_t count = enumTexts_DrawOrders.size();
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
		template<> struct StrToEnumMappings<ObjectAlignment> {
			inline static constexpr std::string_view const* svs = enumTexts_ObjectAlignment.data();
			inline static constexpr size_t count = enumTexts_ObjectAlignment.size();
		};
		inline static constexpr std::array<std::string_view, 2> enumTexts_TileRenderSizes = {
			"tile"sv,
			"grid"sv,
		};
		template<> struct StrToEnumMappings<TileRenderSizes> {
			inline static constexpr std::string_view const* svs = enumTexts_TileRenderSizes.data();
			inline static constexpr size_t count = enumTexts_TileRenderSizes.size();
		};
		inline static constexpr std::array<std::string_view, 2> enumTexts_FillModes = {
			"stretch"sv,
			"preserve-aspect-fit"sv,
		};
		template<> struct StrToEnumMappings<FillModes> {
			inline static constexpr std::string_view const* svs = enumTexts_FillModes.data();
			inline static constexpr size_t count = enumTexts_FillModes.size();
		};


		/**********************************************************************************/

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
				out.r = xx::FromHex(&sv[0]);
				out.g = xx::FromHex(&sv[2]);
				out.b = xx::FromHex(&sv[4]);
				return;
			case 8:
				out.a = xx::FromHex(&sv[0]);
				out.r = xx::FromHex(&sv[2]);
				out.g = xx::FromHex(&sv[4]);
				out.b = xx::FromHex(&sv[6]);
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
			out.reserve(dr.len / 4);
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

		Property* Filler::MakeProperty(std::vector<Property>& out, pugi::xml_node const& c, std::string&& name) {
			auto&& p = &out.emplace_back();
			p->name = std::move(name);
			p->type = PropertyTypes::String;
			TryFill(p->type, c.attribute("type"));
			return p;
		}

		void Filler::TryFillProperties(std::vector<Property>& out, pugi::xml_node const& owner, bool needOverride) {
			for (auto&& c : owner.child("properties").children("property")) {
				Property* p;
				if (needOverride) {
					std::string name;
					TryFill(name, c.attribute("name"));
					auto&& iter = std::find_if(out.begin(), out.end(), [&](Property const& prop) {
						return prop.name == name;
						});
					if (iter == out.end()) {
						p = MakeProperty(out, c, std::move(name));
					} else {
						p = &*iter;
					}
				} else {
					p = MakeProperty(out, c, c.attribute("name").as_string());
				}
				switch (p->type) {
				case PropertyTypes::Bool:
				{
					p->value = c.attribute("value").as_bool();
					break;
				}
				case PropertyTypes::Color:
				{
					RGBA8 o{ 0, 0, 0, 255 };
					TryFill(o, c.attribute("value"));
					p->value = o;
					break;
				}
				case PropertyTypes::Float:
				{
					p->value = c.attribute("value").as_double();
					break;
				}
				case PropertyTypes::File:
				{
					p->value = std::make_unique<std::string>(c.attribute("value").as_string());
					break;
				}
				case PropertyTypes::Int:
				{
					p->value = c.attribute("value").as_llong();
					break;
				}
				case PropertyTypes::Object:
				{
					p->value = c.attribute("value").as_llong();	// convert to xx::Weak<Object> at fill step 2
					objProps.emplace_back(&out, out.size() - 1);
					break;
				}
				case PropertyTypes::String:
				{
					p->value = std::make_unique<std::string>(c.attribute("value").as_string());
					break;
				}
				default:
					throw std::logic_error("FillPropertiesTo error: unhandled property type: " + std::to_string((int)p->type));
				}
			}
		}

		void Filler::TryFillImage(xx::Shared<Image>& out, pugi::xml_node const& c) {
			out.Reset();
			if (c.empty()) return;
			std::string s;
			TryFill(s, c.attribute("source"));
			auto&& fp = rootPath + s;	// to fullpath
			auto&& iter = std::find_if(map.images.begin(), map.images.end(), [&](xx::Shared<Image> const& img) {
				return img->source == s;
				});
			if (iter == map.images.end()) {
				auto&& img = out.Emplace();
				img->source = std::move(s);
				img->texture.Emplace(engine.LoadTexture(fp));
				TryFill(img->width, c.attribute("width"));
				TryFill(img->height, c.attribute("height"));
				TryFill(img->transparentColor, c.attribute("trans"));
				map.images.push_back(img);
			} else {
				out = *iter;
			}
		}

		void Filler::TryFillTileset(Tileset& ts, pugi::xml_node const& c) {
			TryFill(ts.firstgid, c.attribute("firstgid"));
			TryFill(ts.source, c.attribute("source"));

			auto&& fp = rootPath + ts.source;	// to fullpath
			if (auto&& d = engine.ReadAllBytesWithFullPath(fp); !d) {
				throw std::logic_error("read file error: " + fp);
			} else if (auto&& r = docTsx.load_buffer(d.buf, d.len); r.status) {
				throw std::logic_error("docTsx.load_buffer error: " + std::string(r.description()));
			} else {
				auto&& cTileset = docTsx.child("tileset");
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
				TryFillImage(ts.image, cTileset.child("image"));
				TryFill(ts.tilewidth, cTileset.attribute("tilewidth"));
				TryFill(ts.tileheight, cTileset.attribute("tileheight"));
				TryFill(ts.margin, cTileset.attribute("margin"));
				TryFill(ts.spacing, cTileset.attribute("spacing"));
				TryFillProperties(ts.properties, cTileset);

				TryFill(ts.version, cTileset.attribute("version"));
				TryFill(ts.tiledversion, cTileset.attribute("tiledversion"));
				TryFill(ts.tilecount, cTileset.attribute("tilecount"));

				for (auto&& cW : cTileset.child("wangsets").children("wangset")) {
					auto&& w = *ts.wangSets.emplace_back(std::make_unique<WangSet>());
					TryFill(w.name, cW.attribute("name"));
					TryFill(w.type, cW.attribute("type"));
					TryFill(w.tile, cW.attribute("tile"));
					TryFillProperties(w.properties, cW);

					for (auto&& cC : cW.children("wangcolor")) {
						auto&& c = *w.wangColors.emplace_back(std::make_unique<WangColor>());
						c.name = cC.attribute("name").as_string();
						TryFill(c.color, cC.attribute("color"));
						TryFill(c.tile, cC.attribute("tile"));
						TryFill(c.probability, cC.attribute("probability"));
						TryFillProperties(c.properties, cC);
					}

					for (auto&& cT : cW.children("wangtile")) {
						auto&& t = w.wangTiles.emplace_back();
						TryFill(t.tileId, cT.attribute("tileid"));
						t.gid = ts.firstgid + t.tileId;
						FillCsvIntsTo(t.wangIds, cT.attribute("wangid").as_string());
					}
				}

				for (auto&& cT : cTileset.children("tile")) {
					auto&& t = *ts.tiles.emplace_back(std::make_unique<Tile>());
					TryFill(t.id, cT.attribute("id"));
					TryFill(t.class_, cT.attribute("class"));
					TryFillImage(t.image, cT.child("image"));
					TryFillProperties(t.properties, cT);
					if (auto&& cObjs = cT.child("objectgroup"); !cObjs.empty()) {
						TryFillLayer(*t.collisions.Emplace(), cObjs);
					}
					if (auto&& cAnims = cT.child("animation"); !cAnims.empty()) {
						for (auto&& cFrame : cAnims.children("frame")) {
							auto& f = t.animation.emplace_back();
							TryFill(f.tileId, cFrame.attribute("tileid"));
							f.gid = ts.firstgid + f.tileId;
							TryFill(f.duration, cFrame.attribute("duration"));
						}
					}
				}
			}
		}

		void Filler::TryFillLayerBase(Layer& L, pugi::xml_node const& c) {
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

		void Filler::TryFillLayer(Layer_Tile& L, pugi::xml_node const& c) {
			L.type = LayerTypes::TileLayer;
			TryFillLayerBase(L, c);
			auto&& [encoding, compression] = map.tileLayerFormat;
			auto&& cData = c.child("data");
			if (map.infinite) {
				switch (encoding) {
				case Encodings::Csv:
				{
					for (auto&& cChunk : cData.children("chunk")) {
						auto&& chunk = L.chunks.emplace_back();
						TryFillChunkBase(chunk, cChunk);
						FillCsvIntsTo(chunk.gids, cChunk.text().as_string());
					}
					break;
				}
				case Encodings::Base64:
				{
					xx::Data tmp;
					for (auto&& cChunk : cData.children("chunk")) {
						auto&& chunk = L.chunks.emplace_back();
						TryFillChunkBase(chunk, cChunk);
						auto&& bin = xx::Base64Decode(xx::Trim(cChunk.text().as_string()));
						switch (compression) {
						case Compressions::Uncompressed:
						{
							FillBinIntsTo(chunk.gids, { bin.data(), bin.size() });
							break;
						}
						case Compressions::Zstd:
						{
							ZstdDecompress(bin, tmp);
							FillBinIntsTo(chunk.gids, tmp);
							break;
						}
						default:
						{
							throw std::logic_error("unsupported compression: " + std::to_string((int)compression));
						}
						}
					}
					break;
				}
				case Encodings::Xml:
				{
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
					throw std::logic_error("unsupported encoding: " + std::to_string((int)encoding));
				};
			} else {
				switch (map.tileLayerFormat.encoding) {
				case Encodings::Csv:
				{
					FillCsvIntsTo(L.gids, cData.text().as_string());
					break;
				}
				case Encodings::Base64:
				{
					xx::Data tmp;
					auto&& bin = xx::Base64Decode(xx::Trim(cData.text().as_string()));
					switch (map.tileLayerFormat.compression) {
					case Compressions::Uncompressed:
					{
						FillBinIntsTo(L.gids, { bin.data(), bin.size() });
						break;
					}
					case Compressions::Zstd:
					{
						ZstdDecompress(bin, tmp);
						FillBinIntsTo(L.gids, tmp);
						break;
					}
					default:
					{
						throw std::logic_error("unsupported compression: " + std::to_string((int)compression));
					}
					}
					break;
				}
				case Encodings::Xml:
				{
					for (auto&& cTile : cData.children("tile")) {
						L.gids.push_back(cTile.attribute("gid").as_uint());
					}
					break;
				}
				default:
					throw std::logic_error("unsupported encoding: " + std::to_string((int)encoding));
				};
			}
		}

		void Filler::TryFillLayer(Layer_Image& L, pugi::xml_node const& c) {
			L.type = LayerTypes::ImageLayer;
			TryFillLayerBase(L, c);
			TryFill(L.repeatX, c.attribute("repeatx"));
			TryFill(L.repeatY, c.attribute("repeaty"));
			TryFillImage(L.image, c.child("image"));
		}

		void Filler::TryFillObject(xx::Shared<Object>& o, pugi::xml_node const& c) {
			bool needOverrideProperties = o;
			if (auto&& cText = c.child("text"); o && o->type == ObjectTypes::Text || !cText.empty()) {
				xx::Shared<Object_Text> a;
				if (o) {
					a = o.ReinterpretCast<Object_Text>();
				} else {
					a.Emplace();
					a->type = ObjectTypes::Text;
				}
				TryFill(a->width, c.attribute("width"));
				TryFill(a->height, c.attribute("height"));
				TryFill(a->fontfamily, c.attribute("fontfamily"));
				TryFill(a->pixelsize, c.attribute("pixelsize"));
				TryFill(a->wrap, c.attribute("wrap"));
				TryFill(a->color, c.attribute("color"));
				TryFill(a->bold, c.attribute("bold"));
				TryFill(a->italic, c.attribute("italic"));
				TryFill(a->underline, c.attribute("underline"));
				TryFill(a->strikeout, c.attribute("strikeout"));
				TryFill(a->kerning, c.attribute("kerning"));
				TryFill(a->halign, c.attribute("halign"));
				TryFill(a->valign, c.attribute("valign"));
				if (!cText.empty()) a->text = cText.text().as_string();
				o = std::move(a);
			} else if (auto&& cPolygon = c.child("polygon"); o && o->type == ObjectTypes::Polygon || !cPolygon.empty()) {
				xx::Shared<Object_Polygon> a;
				if (o) {
					a = o.ReinterpretCast<Object_Polygon>();
				} else {
					a.Emplace();
					a->type = ObjectTypes::Polygon;
				}
				if (auto&& attr = c.attribute("points"); !attr.empty()) FillPointsTo(a->points, attr.as_string());
				o = std::move(a);
			} else if (auto&& cEllipse = c.child("ellipse"); o && o->type == ObjectTypes::Ellipse || !cEllipse.empty()) {
				xx::Shared<Object_Ellipse> a;
				if (o) {
					a = o.ReinterpretCast<Object_Ellipse>();
				} else {
					a.Emplace();
					a->type = ObjectTypes::Ellipse;
				}
				TryFill(a->width, c.attribute("width"));
				TryFill(a->height, c.attribute("height"));
				o = std::move(a);
			} else if (auto&& cPoint = c.child("point"); o && o->type == ObjectTypes::Point || !cPoint.empty()) {
				xx::Shared<Object_Point> a;
				if (o) {
					a = o.ReinterpretCast<Object_Point>();
				} else {
					a.Emplace();
					a->type = ObjectTypes::Point;
				}
				o = std::move(a);
			} else if (auto&& aGid = c.attribute("gid"); o && o->type == ObjectTypes::Tile || !aGid.empty()) {
				xx::Shared<Object_Tile> a;
				if (o) {
					a = o.ReinterpretCast<Object_Tile>();
				} else {
					a.Emplace();
					a->type = ObjectTypes::Tile;
				}
				TryFill(a->width, c.attribute("width"));
				TryFill(a->height, c.attribute("height"));
				if (auto&& attr = c.attribute("gid"); !attr.empty()) {
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
				TryFill(a->width, c.attribute("width"));
				TryFill(a->height, c.attribute("height"));
				o = std::move(a);
			}
			TryFill(o->id, c.attribute("id"));
			TryFill(o->name, c.attribute("name"));
			TryFill(o->class_, c.attribute("class"));
			TryFill(o->x, c.attribute("x"));
			TryFill(o->y, c.attribute("y"));
			TryFill(o->rotation, c.attribute("rotation"));
			TryFill(o->visible, c.attribute("visible"));
			TryFillProperties(o->properties, c, needOverrideProperties);
			if (o->id) {	// not a template
				objs[o->id] = o;
			}
		}

		void Filler::TryFillLayer(Layer_Object& L, pugi::xml_node const& c) {
			L.type = LayerTypes::ObjectLayer;
			TryFillLayerBase(L, c);
			TryFill(L.color, c.attribute("color"));
			TryFill(L.draworder, c.attribute("draworder"));

			for (auto&& cObject : c.children("object")) {
				auto&& o = L.objects.emplace_back();
				if (auto&& aTemplate = cObject.attribute("template"); !aTemplate.empty()) {
					auto&& fn = rootPath + aTemplate.as_string();
					if (auto&& [d, fp] = engine.ReadAllBytes(fn); !d) {
						throw std::logic_error("read file error: " + fn);
					} else if (auto&& r = docTx.load_buffer(d.buf, d.len); r.status) {
						throw std::logic_error("docTx.load_buffer error: " + std::string(r.description()));
					} else {
						auto&& cObj = docTx.child("template").child("object");
						TryFillObject(o, cObj);
					}
				}
				TryFillObject(o, cObject);
			}
		}

		void Filler::TryFillLayer(Layer_Group& pL, pugi::xml_node const& pC) {
			pL.type = LayerTypes::GroupLayer;
			TryFillLayerBase(pL, pC);
			for (auto&& c : pC.children()) {
				std::string_view name(c.name());
				if (name == "properties"sv) continue;
				if (name == "layer"sv) {
					auto&& L = xx::Make<Layer_Tile>();
					TryFillLayer(*L, c);
					pL.layers.emplace_back(std::move(L));
				} else if (name == "imagelayer"sv) {
					auto&& L = xx::Make<Layer_Image>();
					TryFillLayer(*L, c);
					pL.layers.emplace_back(std::move(L));
				} else if (name == "objectgroup"sv) {
					auto&& L = xx::Make<Layer_Object>();
					TryFillLayer(*L, c);
					pL.layers.emplace_back(std::move(L));
				} else if (name == "group"sv) {
					auto&& L = xx::Make<Layer_Group>();
					TryFillLayer(*L, c);
					pL.layers.emplace_back(std::move(L));
				} else {
					throw std::logic_error("unhandled layer type");
				}
			}
		}

		/**************************************************************************************************/
		Filler::Filler(Map& map, std::string_view tmxfn)
			: map(map) {

			// load file & calc rootPath
			if (auto&& [d, fp] = engine.ReadAllBytes(tmxfn); !d) {
				throw std::logic_error("read file error: " + std::string(tmxfn));
			} else if (auto&& r = docTmx.load_buffer(d.buf, d.len); r.status) {
				throw std::logic_error("docTmx.load_buffer error: " + std::string(r.description()));
			} else {
				if (auto&& i = fp.find_last_of("/"); i != fp.npos) {
					rootPath = fp.substr(0, i + 1);
				}
			}

			// fill step 1: parse xml to struct
			auto&& cMap = docTmx.child("map");
			TryFill(map.class_, cMap.attribute("class"));
			TryFill(map.orientation, cMap.attribute("orientation"));
			TryFill(map.width, cMap.attribute("width"));
			TryFill(map.height, cMap.attribute("height"));
			TryFill(map.tileWidth, cMap.attribute("tilewidth"));
			TryFill(map.tileHeight, cMap.attribute("tileheight"));
			TryFill(map.infinite, cMap.attribute("infinite"));
			TryFill(map.tileSideLength, cMap.attribute("hexsidelength"));
			TryFill(map.staggeraxis, cMap.attribute("staggeraxis"));
			TryFill(map.staggerindex, cMap.attribute("staggerindex"));
			TryFill(map.parallaxOrigin.x, cMap.attribute("parallaxoriginx"));
			TryFill(map.parallaxOrigin.y, cMap.attribute("parallaxoriginy"));
			if (auto&& cLayer = cMap.child("layer"); !cLayer.empty()) {
				if (auto&& cData = cLayer.child("data"); !cData.empty()) {
					TryFill(map.tileLayerFormat.encoding, cData.attribute("encoding"));
					TryFill(map.tileLayerFormat.compression, cData.attribute("compression"));
				}
			}
			if (auto&& cEditorSettings = cMap.child("editorsettings"); !cEditorSettings.empty()) {
				if (auto&& cChunkSize = cEditorSettings.child("chunksize"); !cChunkSize.empty()) {
					TryFill(map.outputChunkWidth, cChunkSize.attribute("width"));
					TryFill(map.outputChunkHeight, cChunkSize.attribute("height"));
				}
			}
			TryFill(map.renderOrder, cMap.attribute("renderorder"));
			TryFill(map.compressionLevel, cMap.attribute("compressionlevel"));
			TryFill(map.backgroundColor, cMap.attribute("backgroundcolor"));
			TryFillProperties(map.properties, cMap);

			TryFill(map.version, cMap.attribute("version"));
			TryFill(map.tiledVersion, cMap.attribute("tiledversion"));
			TryFill(map.nextLayerId, cMap.attribute("nextlayerid"));
			TryFill(map.nextObjectId, cMap.attribute("nextobjectid"));

			for (auto&& c : cMap.children()) {
				std::string_view name(c.name());
				if (name == "tileset"sv) {
					auto&& ts = xx::Make<Tileset>();
					TryFillTileset(*ts, c);
					map.tilesets.emplace_back(std::move(ts));
				} else if (name == "layer"sv) {
					auto&& L = xx::Make<Layer_Tile>();
					TryFillLayer(*L, c);
					map.layers.emplace_back(std::move(L));
				} else if (name == "imagelayer"sv) {
					auto&& L = xx::Make<Layer_Image>();
					TryFillLayer(*L, c);
					map.layers.emplace_back(std::move(L));
				} else if (name == "objectgroup"sv) {
					auto&& L = xx::Make<Layer_Object>();
					TryFillLayer(*L, c);
					map.layers.emplace_back(std::move(L));
				} else if (name == "group"sv) {
					auto&& L = xx::Make<Layer_Group>();
					TryFillLayer(*L, c);
					map.layers.emplace_back(std::move(L));
				} else {
					throw std::logic_error("unhandled layer type");
				}
			}

			// fill step 2: fix object ref

			for (auto&& [ps, idx] : objProps) {
				auto& p = (*ps)[idx];
				p.value = objs[std::get<int64_t>(p.value)];
			}
			objs.clear();
			objProps.clear();


			// fill step 3: fill gid info
			if (map.tilesets.empty()) return;

			uint32_t numCells = 0;
			for (auto&& tileset : map.tilesets) {
				numCells += tileset->tilecount;
			}
			map.gidInfos.resize(numCells + map.tilesets[0]->firstgid);

			for (auto&& tileset : map.tilesets) {
				uint32_t numRows = 1, numCols = tileset->tilecount;
				Image* img = nullptr;
				if (tileset->image) {
					numCols = tileset->columns;
					numRows = tileset->tilecount / numCols;
					img = tileset->image;
				}
				for (uint32_t y = 0; y < numRows; ++y) {
					for (uint32_t x = 0; x < numCols; ++x) {
						auto id = y * numCols + x;
						auto gid = tileset->firstgid + id;
						auto& info = map.gidInfos[gid];
						info.tileset = tileset;
						info.tile = nullptr;
						info.image = img;
						for (auto& t : tileset->tiles) {
							if (t->id == id) {
								info.tile = t.get();
								if (t->image) {
									info.image = t->image;
								}
								break;
							}
						}
						if (info.IsSingleImage()) {
							info.u = 0;
							info.v = 0;
							info.w = info.image->width;
							info.h = info.image->height;
						} else {
							info.u = tileset->margin + (tileset->spacing + tileset->tilewidth) * x;
							info.v = tileset->margin + (tileset->spacing + tileset->tileheight) * y;
							info.w = tileset->tilewidth;
							info.h = tileset->tileheight;
						}
					}
				}
			}
		}

		void FillTo(Map& map, std::string_view const& tmxfn) {
			Filler(map, tmxfn);
		}


		bool GidInfo::IsSingleImage() const {
			return tile && tile->image == image;
		}
	}

}
