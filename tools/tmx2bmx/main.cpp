#include "pch.h"
#include "main.h"

namespace xx {
	struct TmxData : Data {
		using Data::Data;
		std::map<uint32_t, Shared<TMX::Object>> objs;		// for read
		std::unordered_map<TMX::Object*, uint32_t> keys;	// for write
		uint32_t key{};
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<RGBA8, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.r, in.g, in.b, in.a);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.r, out.g, out.b, out.a);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<std::unique_ptr<std::string>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(*in);
		}
		static inline int Read(Data_r& d, T& out) {
			if (!out) {
				out = std::make_unique<std::string>();
			}
			return d.Read(*out);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Property, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.type, in.name);
			switch (in.type) {
			case TMX::PropertyTypes::Bool:
				d.Write<needReserve>(std::get<bool>(in.value));
				break;
			case TMX::PropertyTypes::Color:
				d.Write<needReserve>(std::get<RGBA8>(in.value));
				break;
			case TMX::PropertyTypes::Float:
				d.Write<needReserve>(std::get<double>(in.value));
				break;
			case TMX::PropertyTypes::String:
			case TMX::PropertyTypes::File:
				d.Write<needReserve>(std::get<std::unique_ptr<std::string>>(in.value));
				break;
			case TMX::PropertyTypes::Int:
				d.Write<needReserve>(std::get<int64_t>(in.value));
				break;
			case TMX::PropertyTypes::Object:
				auto& td = (TmxData&)d;
				auto o = std::get<TMX::Object*>(in.value);
				if (auto iter = td.keys.find(o); iter != td.keys.end()) {
					d.Write<needReserve>(iter.second);
				} else {
					td.keys[o] = ++td.key;
					d.Write<needReserve>((uint32_t)0, o->type);
					switch (o->type) {
					case TMX::ObjectTypes::Point:
						d.Write<needReserve>(*(TMX::Object_Point*)o);
						break;
					case TMX::ObjectTypes::Ellipse:
						d.Write<needReserve>(*(TMX::Object_Ellipse*)o);
						break;
					case TMX::ObjectTypes::Polygon:
						d.Write<needReserve>(*(TMX::Object_Polygon*)o);
						break;
					case TMX::ObjectTypes::Rectangle:
						d.Write<needReserve>(*(TMX::Object_Rectangle*)o);
						break;
					case TMX::ObjectTypes::Tile:
						d.Write<needReserve>(*(TMX::Object_Tile*)o);
						break;
					case TMX::ObjectTypes::Text:
						d.Write<needReserve>(*(TMX::Object_Text*)o);
						break;
					default:
						xx_assert(false);
					}
				}
				break;
			default:
				xx_assert(false);
			}
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read(out.type, out.name)) return r;
			switch (out.type) {
			case TMX::PropertyTypes::Bool: {
				bool v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			}
			case TMX::PropertyTypes::Color:
				RGBA8 v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			case TMX::PropertyTypes::Float:
				double v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			case TMX::PropertyTypes::String:
			case TMX::PropertyTypes::File:
				std::unique_ptr<std::string> v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			case TMX::PropertyTypes::Int:
				int64_t v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			case TMX::PropertyTypes::Object:
				auto& td = (TmxData&)d;
				uint32_t key;
				if (int r = d.Read(key)) return r;
				if (key) {
					out.value = td.objs[key].pointer;
				} else {
					TMX::ObjectTypes ot;
					if (int r = d.Read(ot)) return r;
					switch (ot) {
					case TMX::ObjectTypes::Point: {
						auto sp = Make<TMX::Object_Point>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					case TMX::ObjectTypes::Ellipse: {
						auto sp = Make<TMX::Object_Ellipse>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					case TMX::ObjectTypes::Polygon: {
						auto sp = Make<TMX::Object_Polygon>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					case TMX::ObjectTypes::Rectangle: {
						auto sp = Make<TMX::Object_Rectangle>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					case TMX::ObjectTypes::Tile: {
						auto sp = Make<TMX::Object_Tile>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					case TMX::ObjectTypes::Text: {
						auto sp = Make<TMX::Object_Text>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						if (int r = d.Read(*sp)) return r;
						break;
					}
					default:
						xx_assert(false);
					}
				}
				break;
			default:
				xx_assert(false);
			}
			return 0;
		}
	};

	// base class
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.type, in.id, in.name, in.class_, in.x, in.y, in.rotation, in.visible, in.properties);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.type, out.id, out.name, out.class_, out.x, out.y, out.rotation, out.visible, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Point, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object&)in);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read((TMX::Object&)out);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Rectangle, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object&)in);
			d.Write<needReserve>(in.width, in.height);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Object&)out)) return r;
			return d.Read(out.width, out.height);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Ellipse, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read((TMX::Object_Rectangle&)out);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Pointi, std::decay_t<T>> || std::is_same_v<TMX::Pointf, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.x, in.y);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.x, out.y);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Polygon, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object&)in);
			d.Write<needReserve>(points);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Object&)out)) return r;
			return d.Read(out.points);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
			d.Write<needReserve>(in.gid, in.flippingHorizontal, in.flippingVertical);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Object_Rectangle&)out)) return r;
			return d.Read(out.gid, out.flippingHorizontal, out.flippingVertical);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Text, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
			d.Write<needReserve>(in.fontfamily, in.pixelsize, in.color, in.wrap, in.bold, in.italic, in.underline, in.strikeout, in.kerning, in.halign, in.valign, in.text);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Object_Rectangle&)out)) return r;
			return d.Read(out.fontfamily, out.pixelsize, out.color, out.wrap, out.bold, out.italic, out.underline, out.strikeout, out.kerning, out.halign, out.valign, out.text);
		}
	};

	// base class
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.type, in.id, in.name, in.class_, in.visible, in.locked, in.opacity, in.tintColor, in.horizontalOffset, in.verticalOffset, in.parallaxFactor, in.properties);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.type, out.id, out.name, out.class_, out.visible, out.locked, out.opacity, out.tintColor, out.horizontalOffset, out.verticalOffset, out.parallaxFactor, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Chunk, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.gids, in.height, in.width, in.pos);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.gids, out.height, out.width, out.pos);
		}
	};
	
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer_Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Layer)in);
			d.Write<needReserve>(in.chunks, in.gids);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.chunks, out.gids);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Object>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			auto& td = (TmxData&)d;
			if (auto iter = td.keys.find(o.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter.second);
			} else {
				td.keys[o.pointer] = ++td.key;
				d.Write<needReserve>((uint32_t)0, o->type);
				switch (o->type) {
				case TMX::ObjectTypes::Point:
					d.Write<needReserve>(*(TMX::Object_Point*)o);
					break;
				case TMX::ObjectTypes::Ellipse:
					d.Write<needReserve>(*(TMX::Object_Ellipse*)o);
					break;
				case TMX::ObjectTypes::Polygon:
					d.Write<needReserve>(*(TMX::Object_Polygon*)o);
					break;
				case TMX::ObjectTypes::Rectangle:
					d.Write<needReserve>(*(TMX::Object_Rectangle*)o);
					break;
				case TMX::ObjectTypes::Tile:
					d.Write<needReserve>(*(TMX::Object_Tile*)o);
					break;
				case TMX::ObjectTypes::Text:
					d.Write<needReserve>(*(TMX::Object_Text*)o);
					break;
				default:
					xx_assert(false);
				}
			}
		}
		static inline int Read(Data_r& d, T& out) {
			auto& td = (TmxData&)d;
			uint32_t key;
			if (int r = d.Read(key)) return r;
			if (key) {
				out = td.objs[key];
			} else {
				TMX::ObjectTypes ot;
				if (int r = d.Read(ot)) return r;
				switch (ot) {
				case TMX::ObjectTypes::Point:
				{
					auto sp = Make<TMX::Object_Point>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				case TMX::ObjectTypes::Ellipse:
				{
					auto sp = Make<TMX::Object_Ellipse>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				case TMX::ObjectTypes::Polygon:
				{
					auto sp = Make<TMX::Object_Polygon>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				case TMX::ObjectTypes::Rectangle:
				{
					auto sp = Make<TMX::Object_Rectangle>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				case TMX::ObjectTypes::Tile:
				{
					auto sp = Make<TMX::Object_Tile>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				case TMX::ObjectTypes::Text:
				{
					auto sp = Make<TMX::Object_Text>();
					td.objs[++td.key] = sp;
					out = sp;
					if (int r = d.Read(*sp)) return r;
					break;
				}
				default:
					xx_assert(false);
				}
			}
			return 0;
		}
	};
	
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer_Object, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((TMX::Layer)in);
			d.Write<needReserve>(in.color, in.draworder, in.objects);
		}
		static inline int Read(Data_r& d, T& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.color, out.draworder, out.objects);
		}
	};
	
	//template<typename T>
	//struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Map, std::decay_t<T>>>> {
	//	template<bool needReserve = true>
	//	static inline void Write(Data& d, T const& in) {
	//		
	//	}
	//	static inline int Read(Data_r& d, T& out) {
	//		return 0;
	//	}
	//};
}


int main() {
	xx::engine.Init();	// for init root search path

	auto&& cp = std::filesystem::current_path();
	std::cout << "tool: *.tmx -> *.bmx\nworking dir: " << cp.string() << "\npress any key continue...\n";
	std::cin.get();
	int n = 0;
	for (auto&& entry : std::filesystem::/*recursive_*/directory_iterator(cp)) {
		if (!entry.is_regular_file()) continue;
		auto&& p = entry.path();
		if (p.extension() != u8".tmx") continue;
		auto fullpath = std::filesystem::absolute(p).u8string();
		auto newPath = fullpath.substr(0, fullpath.size() - 6) + u8".bmx";

		xx::TMX::Map map;
		xx::TMX::FillTo(map, (std::string&)p.u8string(), false);
		std::cout << "handle file: " << p << std::endl;
		//xx::Data d;
		//d.Write(tp.premultiplyAlpha);
		//d.Write(tp.realTextureFileName);
		//d.Write(tp.frames.size());
		//for (auto& f : tp.frames) {
		//	std::cout << "handle frame: " << f->key << std::endl;
		//	d.Write(f->key);
		//	d.Write(f->anchor);
		//	d.Write((xx::RWFloatUInt16&)f->spriteOffset.x, (xx::RWFloatUInt16&)f->spriteOffset.y);
		//	d.Write((xx::RWFloatUInt16&)f->spriteSize.x, (xx::RWFloatUInt16&)f->spriteSize.y);
		//	d.Write((xx::RWFloatUInt16&)f->spriteSourceSize.x, (xx::RWFloatUInt16&)f->spriteSourceSize.y);
		//	d.Write((xx::RWFloatUInt16&)f->textureRect.x, (xx::RWFloatUInt16&)f->textureRect.y);
		//	d.Write((xx::RWFloatUInt16&)f->textureRect.wh.x, (xx::RWFloatUInt16&)f->textureRect.wh.y);
		//}
		//xx::WriteAllBytes(newPath, d);

		++n;

		//bool premultiplyAlpha;
		//std::string realTextureFileName;
		//size_t numFrames{};
		//if (int r = d.Read(premultiplyAlpha, realTextureFileName, numFrames)) return r;
		//for (size_t i = 0; i < numFrames; ++i) {
		//	std::string frameKey;
		//	std::optional<xx::XY> anchor;
		//	xx::XY spriteOffset, spriteSize, spriteSourceSize;
		//	float x, y, w, h;
		//	if (int r = d.Read(frameKey, anchor)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteOffset.x, (xx::RWFloatUInt16&)spriteOffset.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteSize.x, (xx::RWFloatUInt16&)spriteSize.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteSourceSize.x, (xx::RWFloatUInt16&)spriteSourceSize.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)x, (xx::RWFloatUInt16&)y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)w, (xx::RWFloatUInt16&)h)) return r;
		//}
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
