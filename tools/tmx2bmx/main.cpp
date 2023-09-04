#include "pch.h"
#include "main.h"

namespace xx {
	struct TmxData : Data {
		using Data::Data;
		std::map<int32_t, Shared<TMX::SharedBase>> objs;		// for read
		std::unordered_map<TMX::SharedBase*, int32_t> keys;		// for write
		int32_t key{};	// 0: empty   -1: new     > 0: exists key
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<RGBA8, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, RGBA8 const& in) {
			d.Write<needReserve>(in.r, in.g, in.b, in.a);
		}
		static inline int Read(Data_r& d, RGBA8& out) {
			return d.Read(out.r, out.g, out.b, out.a);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Property, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Property const& in) {
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
			case TMX::PropertyTypes::Object: {
				auto& td = (TmxData&)d;
				auto o = std::get<TMX::Object*>(in.value);
				if (!o) {
					d.Write<needReserve>(0);
				} else if (auto iter = td.keys.find(o); iter != td.keys.end()) {
					d.Write<needReserve>(iter->second);
				} else {
					td.keys[o] = ++td.key;
					d.Write<needReserve>(-1, o->type);
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
			}
			default:
				xx_assert(false);
			}
		}
		static inline int Read(Data_r& d, TMX::Property& out) {
			if (int r = d.Read(out.type, out.name)) return r;
			switch (out.type) {
			case TMX::PropertyTypes::Bool: {
				bool v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			}
			case TMX::PropertyTypes::Color: {
				RGBA8 v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			}
			case TMX::PropertyTypes::Float: {
				double v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			}
			case TMX::PropertyTypes::String:
			case TMX::PropertyTypes::File: {
				std::unique_ptr<std::string> v;
				if (int r = d.Read(v)) return r;
				out.value = std::move(v);
				break;
			}
			case TMX::PropertyTypes::Int: {
				int64_t v;
				if (int r = d.Read(v)) return r;
				out.value = v;
				break;
			}
			case TMX::PropertyTypes::Object: {
				auto& td = (TmxData&)d;
				int32_t key;
				if (int r = d.Read(key)) return r;
				if (key == 0) {
					out.value = (TMX::Object*)0;
				} else if (key > 0) {
					out.value = (TMX::Object*)td.objs[key].pointer;
				} else {
					TMX::ObjectTypes ot;
					if (int r = d.Read(ot)) return r;
					switch (ot) {
					case TMX::ObjectTypes::Point: {
						auto sp = Make<TMX::Object_Point>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					case TMX::ObjectTypes::Ellipse: {
						auto sp = Make<TMX::Object_Ellipse>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					case TMX::ObjectTypes::Polygon: {
						auto sp = Make<TMX::Object_Polygon>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					case TMX::ObjectTypes::Rectangle: {
						auto sp = Make<TMX::Object_Rectangle>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					case TMX::ObjectTypes::Tile: {
						auto sp = Make<TMX::Object_Tile>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					case TMX::ObjectTypes::Text: {
						auto sp = Make<TMX::Object_Text>();
						td.objs[++td.key] = sp;
						out.value = sp.pointer;
						return d.Read(*sp);
					}
					default:
						xx_assert(false);
					}
				}
				break;
			}
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
		static inline void Write(Data& d, TMX::Object const& in) {
			d.Write<needReserve>(in.type, in.id, in.name, in.class_, in.x, in.y, in.rotation, in.visible, in.properties);
		}
		static inline int Read(Data_r& d, TMX::Object& out) {
			return d.Read(out.type, out.id, out.name, out.class_, out.x, out.y, out.rotation, out.visible, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Point, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Object_Point const& in) {
			d.Write<needReserve>((TMX::Object&)in);
		}
		static inline int Read(Data_r& d, TMX::Object_Point& out) {
			return d.Read((TMX::Object&)out);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Rectangle, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Object_Rectangle const& in) {
			d.Write<needReserve>((TMX::Object&)in);
			d.Write<needReserve>(in.width, in.height);
		}
		static inline int Read(Data_r& d, TMX::Object_Rectangle& out) {
			if (int r = d.Read((TMX::Object&)out)) return r;
			return d.Read(out.width, out.height);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Ellipse, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Object_Ellipse const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
		}
		static inline int Read(Data_r& d, TMX::Object_Ellipse& out) {
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
		static inline void Write(Data& d, TMX::Object_Polygon const& in) {
			d.Write<needReserve>((TMX::Object&)in);
			d.Write<needReserve>(in.points);
		}
		static inline int Read(Data_r& d, TMX::Object_Polygon& out) {
			if (int r = d.Read((TMX::Object&)out)) return r;
			return d.Read(out.points);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Object_Tile const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
			d.Write<needReserve>(in.gid, in.flippingHorizontal, in.flippingVertical);
		}
		static inline int Read(Data_r& d, TMX::Object_Tile& out) {
			if (int r = d.Read((TMX::Object_Rectangle&)out)) return r;
			return d.Read(out.gid, out.flippingHorizontal, out.flippingVertical);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Object_Text, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Object_Text const& in) {
			d.Write<needReserve>((TMX::Object_Rectangle&)in);
			d.Write<needReserve>(in.fontfamily, in.pixelsize, in.color, in.wrap, in.bold, in.italic, in.underline, in.strikeout, in.kerning, in.halign, in.valign, in.text);
		}
		static inline int Read(Data_r& d, TMX::Object_Text& out) {
			if (int r = d.Read((TMX::Object_Rectangle&)out)) return r;
			return d.Read(out.fontfamily, out.pixelsize, out.color, out.wrap, out.bold, out.italic, out.underline, out.strikeout, out.kerning, out.halign, out.valign, out.text);
		}
	};

	// base class
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Layer const& in) {
			d.Write<needReserve>(in.type, in.id, in.name, in.class_, in.visible, in.locked, in.opacity, in.tintColor, in.horizontalOffset, in.verticalOffset, in.parallaxFactor, in.properties);
		}
		static inline int Read(Data_r& d, TMX::Layer& out) {
			return d.Read(out.type, out.id, out.name, out.class_, out.visible, out.locked, out.opacity, out.tintColor, out.horizontalOffset, out.verticalOffset, out.parallaxFactor, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Chunk, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Chunk const& in) {
			d.Write<needReserve>(in.gids, in.height, in.width, in.pos);
		}
		static inline int Read(Data_r& d, TMX::Chunk& out) {
			return d.Read(out.gids, out.height, out.width, out.pos);
		}
	};
	
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer_Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Layer_Tile const& in) {
			d.Write<needReserve>((TMX::Layer&)in);
			d.Write<needReserve>(in.chunks, in.gids);
		}
		static inline int Read(Data_r& d, TMX::Layer_Tile& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.chunks, out.gids);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Object>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, Shared<TMX::Object> const& in) {
			auto& td = (TmxData&)d;
			if (!in) {
				d.Write<needReserve>(0);
			} else if (auto iter = td.keys.find(in.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter->second);
			} else {
				td.keys[in.pointer] = ++td.key;
				d.Write<needReserve>(-1, in->type);
				switch (in->type) {
				case TMX::ObjectTypes::Point:
					d.Write<needReserve>(*(TMX::Object_Point*)in.pointer);
					break;
				case TMX::ObjectTypes::Ellipse:
					d.Write<needReserve>(*(TMX::Object_Ellipse*)in.pointer);
					break;
				case TMX::ObjectTypes::Polygon:
					d.Write<needReserve>(*(TMX::Object_Polygon*)in.pointer);
					break;
				case TMX::ObjectTypes::Rectangle:
					d.Write<needReserve>(*(TMX::Object_Rectangle*)in.pointer);
					break;
				case TMX::ObjectTypes::Tile:
					d.Write<needReserve>(*(TMX::Object_Tile*)in.pointer);
					break;
				case TMX::ObjectTypes::Text:
					d.Write<needReserve>(*(TMX::Object_Text*)in.pointer);
					break;
				default:
					xx_assert(false);
				}
			}
		}
		static inline int Read(Data_r& d, Shared<TMX::Object>& out) {
			auto& td = (TmxData&)d;
			int32_t key;
			if (int r = d.Read(key)) return r;
			if (key == 0) {
				out.Reset();
			} else if (key > 0) {
				out = td.objs[key].ReinterpretCast<TMX::Object>();
			} else {
				TMX::ObjectTypes ot;
				if (int r = d.Read(ot)) return r;
				switch (ot) {
				case TMX::ObjectTypes::Point: {
					auto sp = Make<TMX::Object_Point>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::ObjectTypes::Ellipse: {
					auto sp = Make<TMX::Object_Ellipse>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::ObjectTypes::Polygon: {
					auto sp = Make<TMX::Object_Polygon>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::ObjectTypes::Rectangle: {
					auto sp = Make<TMX::Object_Rectangle>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::ObjectTypes::Tile: {
					auto sp = Make<TMX::Object_Tile>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::ObjectTypes::Text: {
					auto sp = Make<TMX::Object_Text>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
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
		static inline void Write(Data& d, TMX::Layer_Object const& in) {
			d.Write<needReserve>((TMX::Layer&)in);
			d.Write<needReserve>(in.color, in.draworder, in.objects);
		}
		static inline int Read(Data_r& d, TMX::Layer_Object& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.color, out.draworder, out.objects);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Layer>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, Shared<TMX::Layer> const& in) {
			auto& td = (TmxData&)d;
			if (!in) {
				d.Write<needReserve>(0);
			} else if (auto iter = td.keys.find(in.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter->second);
			} else {
				td.keys[in.pointer] = ++td.key;
				d.Write<needReserve>(-1, in->type);
				switch (in->type) {
				case TMX::LayerTypes::TileLayer:
					d.Write<needReserve>(*(TMX::Layer_Tile*)in.pointer);
					break;
				case TMX::LayerTypes::ObjectLayer:
					d.Write<needReserve>(*(TMX::Layer_Object*)in.pointer);
					break;
				case TMX::LayerTypes::ImageLayer:
					d.Write<needReserve>(*(TMX::Layer_Image*)in.pointer);
					break;
				case TMX::LayerTypes::GroupLayer:
					d.Write<needReserve>(*(TMX::Layer_Group*)in.pointer);
					break;
				default:
					xx_assert(false);
				}
			}
		}
		static inline int Read(Data_r& d, Shared<TMX::Layer>& out) {
			auto& td = (TmxData&)d;
			int32_t key;
			if (int r = d.Read(key)) return r;
			if (key == 0) {
				out.Reset();
			} else if (key > 0) {
				out = td.objs[key].ReinterpretCast<TMX::Layer>();
			} else {
				TMX::LayerTypes ot;
				if (int r = d.Read(ot)) return r;
				switch (ot) {
				case TMX::LayerTypes::TileLayer: {
					auto sp = Make<TMX::Layer_Tile>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::LayerTypes::ObjectLayer: {
					auto sp = Make<TMX::Layer_Object>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::LayerTypes::ImageLayer: {
					auto sp = Make<TMX::Layer_Image>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				case TMX::LayerTypes::GroupLayer: {
					auto sp = Make<TMX::Layer_Group>();
					td.objs[++td.key] = sp;
					out = sp;
					return d.Read(*sp);
				}
				default:
					xx_assert(false);
				}
			}
			return 0;
		}
	};
	
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer_Group, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Layer_Group const& in) {
			d.Write<needReserve>((TMX::Layer&)in);
			d.Write<needReserve>(in.layers);
		}
		static inline int Read(Data_r& d, TMX::Layer_Group& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.layers);
		}
	};
	
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Image>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, Shared<TMX::Image> const& in) {
			auto& td = (TmxData&)d;
			if (!in) {
				d.Write<needReserve>(0);
			} else if (auto iter = td.keys.find(in.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter->second);
			} else {
				td.keys[in.pointer] = ++td.key;
				d.Write<needReserve>(-1, in->source, in->width, in->height, in->transparentColor);
			}
		}
		static inline int Read(Data_r& d, Shared<TMX::Image>& out) {
			auto& td = (TmxData&)d;
			int32_t key;
			if (int r = d.Read(key)) return r;
			if (key == 0) {
				out.Reset();
			} else if (key > 0) {
				out = td.objs[key].ReinterpretCast<TMX::Image>();
			} else {
				auto sp = Make<TMX::Image>();
				td.objs[++td.key] = sp;
				out = sp;
				return d.Read(out->source, out->width, out->height, out->transparentColor);
			}
			return 0;
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Layer_Image, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Layer_Image const& in) {
			d.Write<needReserve>((TMX::Layer&)in);
			d.Write<needReserve>(in.image, in.repeatX, in.repeatY);
		}
		static inline int Read(Data_r& d, TMX::Layer_Image& out) {
			if (int r = d.Read((TMX::Layer&)out)) return r;
			return d.Read(out.image, out.repeatX, out.repeatY);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Frame, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Frame const& in) {
			d.Write<needReserve>(in.tileId, in.gid, in.duration);
		}
		static inline int Read(Data_r& d, TMX::Frame& out) {
			return d.Read(out.tileId, out.gid, out.duration);
		}
	};


	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Layer_Object>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, Shared<TMX::Layer_Object> const& in) {
			auto& td = (TmxData&)d;
			if (!in) {
				d.Write<needReserve>(0);
			} else if (auto iter = td.keys.find(in.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter->second);
			} else {
				td.keys[in.pointer] = ++td.key;
				d.Write<needReserve>(-1, *in);
			}
		}
		static inline int Read(Data_r& d, Shared<TMX::Layer_Object>& out) {
			auto& td = (TmxData&)d;
			int32_t key;
			if (int r = d.Read(key)) return r;
			if (key == 0) {
				out.Reset();
			} else if (key > 0) {
				out = td.objs[key].ReinterpretCast<TMX::Layer_Object>();
			} else {
				auto sp = Make<TMX::Layer_Object>();
				td.objs[++td.key] = sp;
				out = sp;
				return d.Read(*out);
			}
			return 0;
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Tile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Tile const& in) {
			d.Write<needReserve>(in.id, in.class_, in.image, in.collisions, in.animation, in.properties);
		}
		static inline int Read(Data_r& d, TMX::Tile& out) {
			return d.Read(out.id, out.class_, out.image, out.collisions, out.animation, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::WangTile, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::WangTile const& in) {
			d.Write<needReserve>(in.tileId, in.gid, in.wangIds);
		}
		static inline int Read(Data_r& d, TMX::WangTile& out) {
			return d.Read(out.tileId, out.gid, out.wangIds);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::WangColor, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::WangColor const& in) {
			d.Write<needReserve>(in.name, in.color, in.tile, in.probability, in.properties);
		}
		static inline int Read(Data_r& d, TMX::WangColor& out) {
			return d.Read(out.name, out.color, out.tile, out.probability, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::WangSet, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::WangSet const& in) {
			d.Write<needReserve>(in.name, in.type, in.tile, in.wangTiles, in.wangColors, in.properties);
		}
		static inline int Read(Data_r& d, TMX::WangSet& out) {
			return d.Read(out.name, out.type, out.tile, out.wangTiles, out.wangColors, out.properties);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Transformations, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Transformations const& in) {
			d.Write<needReserve>(in.flipHorizontally, in.flipVertically, in.rotate, in.preferUntransformedTiles);
		}
		static inline int Read(Data_r& d, TMX::Transformations& out) {
			return d.Read(out.flipHorizontally, out.flipVertically, out.rotate, out.preferUntransformedTiles);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<Shared<TMX::Tileset>, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, Shared<TMX::Tileset> const& in) {
			auto& td = (TmxData&)d;
			if (!in) {
				d.Write<needReserve>(0);
			} else if (auto iter = td.keys.find(in.pointer); iter != td.keys.end()) {
				d.Write<needReserve>(iter->second);
			} else {
				td.keys[in.pointer] = ++td.key;
				d.Write<needReserve>(-1);
				d.Write<needReserve>(in->firstgid
					, in->source, in->name, in->class_, in->objectAlignment, in->drawingOffset
					, in->tileRenderSize, in->fillMode, in->backgroundColor, in->orientation, in->gridWidth, in->gridHeight
					, in->columns, in->allowedTransformations, in->image, in->tilewidth, in->tileheight, in->margin
					, in->spacing, in->properties, in->version, in->tiledversion, in->tilecount, in->wangSets, in->tiles
				);
			}
		}
		static inline int Read(Data_r& d, Shared<TMX::Tileset>& out) {
			auto& td = (TmxData&)d;
			int32_t key;
			if (int r = d.Read(key)) return r;
			if (key == 0) {
				out.Reset();
			} else if (key > 0) {
				out = td.objs[key].ReinterpretCast<TMX::Tileset>();
			} else {
				auto sp = Make<TMX::Tileset>();
				td.objs[++td.key] = sp;
				out = sp;
				return d.Read(out->firstgid, out->source, out->name, out->class_, out->objectAlignment, out->drawingOffset
					, out->tileRenderSize, out->fillMode, out->backgroundColor, out->orientation, out->gridWidth, out->gridHeight
					, out->columns, out->allowedTransformations, out->image, out->tilewidth, out->tileheight, out->margin
					, out->spacing, out->properties, out->version, out->tiledversion, out->tilecount, out->wangSets, out->tiles
				);
			}
			return 0;
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::TileLayerFormat, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::TileLayerFormat const& in) {
			d.Write<needReserve>(in.encoding, in.compression);
		}
		static inline int Read(Data_r& d, TMX::TileLayerFormat& out) {
			return d.Read(out.encoding, out.compression);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<TMX::Map, std::decay_t<T>>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, TMX::Map const& in) {
			d.Write<needReserve>(in.class_, in.orientation, in.width, in.height, in.tileWidth, in.tileHeight
				, in.infinite, in.tileSideLength, in.staggeraxis, in.staggerindex, in.parallaxOrigin, in.tileLayerFormat
				, in.outputChunkWidth, in.outputChunkHeight, in.renderOrder, in.compressionLevel, in.backgroundColor, in.properties
				, in.version, in.tiledVersion, in.nextLayerId, in.nextObjectId, in.tilesets, in.layers, in.images
			);
		}
		static inline int Read(Data_r& d, TMX::Map& out) {
			if (int r = d.Read(out.class_, out.orientation, out.width, out.height, out.tileWidth, out.tileHeight
				, out.infinite, out.tileSideLength, out.staggeraxis, out.staggerindex, out.parallaxOrigin, out.tileLayerFormat
				, out.outputChunkWidth, out.outputChunkHeight, out.renderOrder, out.compressionLevel, out.backgroundColor, out.properties
				, out.version, out.tiledVersion, out.nextLayerId, out.nextObjectId, out.tilesets, out.layers, out.images
			)) return r;

			if (out.tilesets.empty()) return 0;

			uint32_t numCells = 0;
			for (auto&& tileset : out.tilesets) {
				numCells += tileset->tilecount;
			}
			out.gidInfos.resize(numCells + out.tilesets[0]->firstgid);

			for (auto&& tileset : out.tilesets) {
				uint32_t numRows = 1, numCols = tileset->tilecount;
				TMX::Image* img = nullptr;
				if (tileset->image) {
					numCols = tileset->columns;
					numRows = tileset->tilecount / numCols;
					img = tileset->image;
				}
				for (uint32_t y = 0; y < numRows; ++y) {
					for (uint32_t x = 0; x < numCols; ++x) {
						auto id = y * numCols + x;
						auto gid = tileset->firstgid + id;
						auto& info = out.gidInfos[gid];
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

			return 0;
		}
	};

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
		auto newPath = fullpath.substr(0, fullpath.size() - 4) + u8".bmx";

		xx::TMX::Map map;
		xx::TMX::FillTo(map, (std::string&)p.u8string(), false);
		std::cout << "handle file: " << p << std::endl;
		xx::TmxData d;
		d.Write(map);
		xx::WriteAllBytes(newPath, d);

		xx::TMX::Map map2;
		d.key = {};
		d.objs.clear();
		d.keys.clear();
		int r = d.Read(map2);
		xx_assert(!r);
		xx_assert(d.offset == d.len);
		// todo: more verify ?

		++n;
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
