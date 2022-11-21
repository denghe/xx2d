#include <tiledmap.h>
#include <tiledmap.cpp.inc>
void CodeGen_tiledmap::Register() {
}
namespace xx {
	void ObjFuncs<::tiledmap_original::Property, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Property const& in) {
        d.Write(in.name);
        d.Write(in.type);
        d.Write(in.value);
    }
	void ObjFuncs<::tiledmap_original::Property, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Property const& in) {
        d.Write<false>(in.name);
        d.Write<false>(in.type);
        d.Write<false>(in.value);
    }
	int ObjFuncs<::tiledmap_original::Property, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Property& out) {
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.type)) return r;
        if (int r = d.Read(out.value)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Property, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Property const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Property, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Property const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"name\":", in.name); 
        om.Append(s, ",\"type\":", in.type);
        om.Append(s, ",\"value\":", in.value);
#endif
    }
    void ObjFuncs<::tiledmap_original::Property>::Clone(::xx::ObjManager& om, ::tiledmap_original::Property const& in, ::tiledmap_original::Property &out) {
        om.Clone_(in.name, out.name);
        om.Clone_(in.type, out.type);
        om.Clone_(in.value, out.value);
    }
    int ObjFuncs<::tiledmap_original::Property>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Property const& in) {
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        if (int r = om.RecursiveCheck(in.value)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Property>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Property& in) {
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.type);
        om.RecursiveReset(in.value);
    }
    void ObjFuncs<::tiledmap_original::Property>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Property& in) {
        om.SetDefaultValue(in.name);
        om.SetDefaultValue(in.type);
        om.SetDefaultValue(in.value);
    }
	void ObjFuncs<::tiledmap_original::Point, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Point const& in) {
        d.Write(in.x);
        d.Write(in.y);
    }
	void ObjFuncs<::tiledmap_original::Point, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Point const& in) {
        d.Write<false>(in.x);
        d.Write<false>(in.y);
    }
	int ObjFuncs<::tiledmap_original::Point, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Point& out) {
        if (int r = d.Read(out.x)) return r;
        if (int r = d.Read(out.y)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Point, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Point const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Point, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Point const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"x\":", in.x); 
        om.Append(s, ",\"y\":", in.y);
#endif
    }
    void ObjFuncs<::tiledmap_original::Point>::Clone(::xx::ObjManager& om, ::tiledmap_original::Point const& in, ::tiledmap_original::Point &out) {
        om.Clone_(in.x, out.x);
        om.Clone_(in.y, out.y);
    }
    int ObjFuncs<::tiledmap_original::Point>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Point const& in) {
        if (int r = om.RecursiveCheck(in.x)) return r;
        if (int r = om.RecursiveCheck(in.y)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Point>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Point& in) {
        om.RecursiveReset(in.x);
        om.RecursiveReset(in.y);
    }
    void ObjFuncs<::tiledmap_original::Point>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Point& in) {
        in.x = 0;
        in.y = 0;
    }
	void ObjFuncs<::tiledmap_original::Chunk, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Chunk const& in) {
        d.Write(in.data);
        d.Write(in.height);
        d.Write(in.width);
        d.Write(in.x);
        d.Write(in.y);
    }
	void ObjFuncs<::tiledmap_original::Chunk, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Chunk const& in) {
        d.Write<false>(in.data);
        d.Write<false>(in.height);
        d.Write<false>(in.width);
        d.Write<false>(in.x);
        d.Write<false>(in.y);
    }
	int ObjFuncs<::tiledmap_original::Chunk, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Chunk& out) {
        if (int r = d.Read(out.data)) return r;
        if (int r = d.Read(out.height)) return r;
        if (int r = d.Read(out.width)) return r;
        if (int r = d.Read(out.x)) return r;
        if (int r = d.Read(out.y)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Chunk, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Chunk const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Chunk, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Chunk const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"data\":", in.data); 
        om.Append(s, ",\"height\":", in.height);
        om.Append(s, ",\"width\":", in.width);
        om.Append(s, ",\"x\":", in.x);
        om.Append(s, ",\"y\":", in.y);
#endif
    }
    void ObjFuncs<::tiledmap_original::Chunk>::Clone(::xx::ObjManager& om, ::tiledmap_original::Chunk const& in, ::tiledmap_original::Chunk &out) {
        om.Clone_(in.data, out.data);
        om.Clone_(in.height, out.height);
        om.Clone_(in.width, out.width);
        om.Clone_(in.x, out.x);
        om.Clone_(in.y, out.y);
    }
    int ObjFuncs<::tiledmap_original::Chunk>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Chunk const& in) {
        if (int r = om.RecursiveCheck(in.data)) return r;
        if (int r = om.RecursiveCheck(in.height)) return r;
        if (int r = om.RecursiveCheck(in.width)) return r;
        if (int r = om.RecursiveCheck(in.x)) return r;
        if (int r = om.RecursiveCheck(in.y)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Chunk>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Chunk& in) {
        om.RecursiveReset(in.data);
        om.RecursiveReset(in.height);
        om.RecursiveReset(in.width);
        om.RecursiveReset(in.x);
        om.RecursiveReset(in.y);
    }
    void ObjFuncs<::tiledmap_original::Chunk>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Chunk& in) {
        om.SetDefaultValue(in.data);
        in.height = 0;
        in.width = 0;
        in.x = 0;
        in.y = 0;
    }
	void ObjFuncs<::tiledmap_original::Object, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Object const& in) {
        d.Write(in.ellipse);
        d.Write(in.gid);
        d.Write(in.height);
        d.Write(in.id);
        d.Write(in.name);
        d.Write(in.point);
        d.Write(in.polygon);
        d.Write(in.polyline);
        d.Write(in.properties);
        d.Write(in.rotation);
        d.Write(in.template_);
        d.Write(in.text);
        d.Write(in.type);
        d.Write(in.visible);
        d.Write(in.width);
        d.Write(in.x);
        d.Write(in.y);
    }
	void ObjFuncs<::tiledmap_original::Object, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Object const& in) {
        d.Write<false>(in.ellipse);
        d.Write<false>(in.gid);
        d.Write<false>(in.height);
        d.Write<false>(in.id);
        d.Write<false>(in.name);
        d.Write<false>(in.point);
        d.Write<false>(in.polygon);
        d.Write<false>(in.polyline);
        d.Write<false>(in.properties);
        d.Write<false>(in.rotation);
        d.Write<false>(in.template_);
        d.Write<false>(in.text);
        d.Write<false>(in.type);
        d.Write<false>(in.visible);
        d.Write<false>(in.width);
        d.Write<false>(in.x);
        d.Write<false>(in.y);
    }
	int ObjFuncs<::tiledmap_original::Object, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Object& out) {
        if (int r = d.Read(out.ellipse)) return r;
        if (int r = d.Read(out.gid)) return r;
        if (int r = d.Read(out.height)) return r;
        if (int r = d.Read(out.id)) return r;
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.point)) return r;
        if (int r = d.Read(out.polygon)) return r;
        if (int r = d.Read(out.polyline)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.rotation)) return r;
        if (int r = d.Read(out.template_)) return r;
        if (int r = d.Read(out.text)) return r;
        if (int r = d.Read(out.type)) return r;
        if (int r = d.Read(out.visible)) return r;
        if (int r = d.Read(out.width)) return r;
        if (int r = d.Read(out.x)) return r;
        if (int r = d.Read(out.y)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Object, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Object const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Object, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Object const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"ellipse\":", in.ellipse); 
        om.Append(s, ",\"gid\":", in.gid);
        om.Append(s, ",\"height\":", in.height);
        om.Append(s, ",\"id\":", in.id);
        om.Append(s, ",\"name\":", in.name);
        om.Append(s, ",\"point\":", in.point);
        om.Append(s, ",\"polygon\":", in.polygon);
        om.Append(s, ",\"polyline\":", in.polyline);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"rotation\":", in.rotation);
        om.Append(s, ",\"template_\":", in.template_);
        om.Append(s, ",\"text\":", in.text);
        om.Append(s, ",\"type\":", in.type);
        om.Append(s, ",\"visible\":", in.visible);
        om.Append(s, ",\"width\":", in.width);
        om.Append(s, ",\"x\":", in.x);
        om.Append(s, ",\"y\":", in.y);
#endif
    }
    void ObjFuncs<::tiledmap_original::Object>::Clone(::xx::ObjManager& om, ::tiledmap_original::Object const& in, ::tiledmap_original::Object &out) {
        om.Clone_(in.ellipse, out.ellipse);
        om.Clone_(in.gid, out.gid);
        om.Clone_(in.height, out.height);
        om.Clone_(in.id, out.id);
        om.Clone_(in.name, out.name);
        om.Clone_(in.point, out.point);
        om.Clone_(in.polygon, out.polygon);
        om.Clone_(in.polyline, out.polyline);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.rotation, out.rotation);
        om.Clone_(in.template_, out.template_);
        om.Clone_(in.text, out.text);
        om.Clone_(in.type, out.type);
        om.Clone_(in.visible, out.visible);
        om.Clone_(in.width, out.width);
        om.Clone_(in.x, out.x);
        om.Clone_(in.y, out.y);
    }
    int ObjFuncs<::tiledmap_original::Object>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Object const& in) {
        if (int r = om.RecursiveCheck(in.ellipse)) return r;
        if (int r = om.RecursiveCheck(in.gid)) return r;
        if (int r = om.RecursiveCheck(in.height)) return r;
        if (int r = om.RecursiveCheck(in.id)) return r;
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.point)) return r;
        if (int r = om.RecursiveCheck(in.polygon)) return r;
        if (int r = om.RecursiveCheck(in.polyline)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.rotation)) return r;
        if (int r = om.RecursiveCheck(in.template_)) return r;
        if (int r = om.RecursiveCheck(in.text)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        if (int r = om.RecursiveCheck(in.visible)) return r;
        if (int r = om.RecursiveCheck(in.width)) return r;
        if (int r = om.RecursiveCheck(in.x)) return r;
        if (int r = om.RecursiveCheck(in.y)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Object>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Object& in) {
        om.RecursiveReset(in.ellipse);
        om.RecursiveReset(in.gid);
        om.RecursiveReset(in.height);
        om.RecursiveReset(in.id);
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.point);
        om.RecursiveReset(in.polygon);
        om.RecursiveReset(in.polyline);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.rotation);
        om.RecursiveReset(in.template_);
        om.RecursiveReset(in.text);
        om.RecursiveReset(in.type);
        om.RecursiveReset(in.visible);
        om.RecursiveReset(in.width);
        om.RecursiveReset(in.x);
        om.RecursiveReset(in.y);
    }
    void ObjFuncs<::tiledmap_original::Object>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Object& in) {
        in.ellipse = false;
        in.gid = 0;
        in.height = 0;
        in.id = 0;
        om.SetDefaultValue(in.name);
        in.point = false;
        om.SetDefaultValue(in.polygon);
        om.SetDefaultValue(in.polyline);
        om.SetDefaultValue(in.properties);
        in.rotation = 0;
        om.SetDefaultValue(in.template_);
        om.SetDefaultValue(in.text);
        om.SetDefaultValue(in.type);
        in.visible = false;
        in.width = 0;
        in.x = 0;
        in.y = 0;
    }
	void ObjFuncs<::tiledmap_original::Layer, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Layer const& in) {
        d.Write(in.chunks);
        d.Write(in.compression);
        d.Write(in.data);
        d.Write(in.draworder);
        d.Write(in.encoding);
        d.Write(in.height);
        d.Write(in.id);
        d.Write(in.image);
        d.Write(in.layers);
        d.Write(in.name);
        d.Write(in.objects);
        d.Write(in.offsetx);
        d.Write(in.offsety);
        d.Write(in.opacity);
        d.Write(in.parallaxx);
        d.Write(in.parallaxy);
        d.Write(in.properties);
        d.Write(in.startx);
        d.Write(in.starty);
        d.Write(in.tintcolor);
        d.Write(in.transparentcolor);
        d.Write(in.type);
        d.Write(in.visible);
        d.Write(in.width);
        d.Write(in.x);
        d.Write(in.y);
    }
	void ObjFuncs<::tiledmap_original::Layer, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Layer const& in) {
        d.Write<false>(in.chunks);
        d.Write<false>(in.compression);
        d.Write<false>(in.data);
        d.Write<false>(in.draworder);
        d.Write<false>(in.encoding);
        d.Write<false>(in.height);
        d.Write<false>(in.id);
        d.Write<false>(in.image);
        d.Write<false>(in.layers);
        d.Write<false>(in.name);
        d.Write<false>(in.objects);
        d.Write<false>(in.offsetx);
        d.Write<false>(in.offsety);
        d.Write<false>(in.opacity);
        d.Write<false>(in.parallaxx);
        d.Write<false>(in.parallaxy);
        d.Write<false>(in.properties);
        d.Write<false>(in.startx);
        d.Write<false>(in.starty);
        d.Write<false>(in.tintcolor);
        d.Write<false>(in.transparentcolor);
        d.Write<false>(in.type);
        d.Write<false>(in.visible);
        d.Write<false>(in.width);
        d.Write<false>(in.x);
        d.Write<false>(in.y);
    }
	int ObjFuncs<::tiledmap_original::Layer, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Layer& out) {
        if (int r = d.Read(out.chunks)) return r;
        if (int r = d.Read(out.compression)) return r;
        if (int r = d.Read(out.data)) return r;
        if (int r = d.Read(out.draworder)) return r;
        if (int r = d.Read(out.encoding)) return r;
        if (int r = d.Read(out.height)) return r;
        if (int r = d.Read(out.id)) return r;
        if (int r = d.Read(out.image)) return r;
        if (int r = d.Read(out.layers)) return r;
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.objects)) return r;
        if (int r = d.Read(out.offsetx)) return r;
        if (int r = d.Read(out.offsety)) return r;
        if (int r = d.Read(out.opacity)) return r;
        if (int r = d.Read(out.parallaxx)) return r;
        if (int r = d.Read(out.parallaxy)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.startx)) return r;
        if (int r = d.Read(out.starty)) return r;
        if (int r = d.Read(out.tintcolor)) return r;
        if (int r = d.Read(out.transparentcolor)) return r;
        if (int r = d.Read(out.type)) return r;
        if (int r = d.Read(out.visible)) return r;
        if (int r = d.Read(out.width)) return r;
        if (int r = d.Read(out.x)) return r;
        if (int r = d.Read(out.y)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Layer, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Layer const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Layer, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Layer const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"chunks\":", in.chunks); 
        om.Append(s, ",\"compression\":", in.compression);
        om.Append(s, ",\"data\":", in.data);
        om.Append(s, ",\"draworder\":", in.draworder);
        om.Append(s, ",\"encoding\":", in.encoding);
        om.Append(s, ",\"height\":", in.height);
        om.Append(s, ",\"id\":", in.id);
        om.Append(s, ",\"image\":", in.image);
        om.Append(s, ",\"layers\":", in.layers);
        om.Append(s, ",\"name\":", in.name);
        om.Append(s, ",\"objects\":", in.objects);
        om.Append(s, ",\"offsetx\":", in.offsetx);
        om.Append(s, ",\"offsety\":", in.offsety);
        om.Append(s, ",\"opacity\":", in.opacity);
        om.Append(s, ",\"parallaxx\":", in.parallaxx);
        om.Append(s, ",\"parallaxy\":", in.parallaxy);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"startx\":", in.startx);
        om.Append(s, ",\"starty\":", in.starty);
        om.Append(s, ",\"tintcolor\":", in.tintcolor);
        om.Append(s, ",\"transparentcolor\":", in.transparentcolor);
        om.Append(s, ",\"type\":", in.type);
        om.Append(s, ",\"visible\":", in.visible);
        om.Append(s, ",\"width\":", in.width);
        om.Append(s, ",\"x\":", in.x);
        om.Append(s, ",\"y\":", in.y);
#endif
    }
    void ObjFuncs<::tiledmap_original::Layer>::Clone(::xx::ObjManager& om, ::tiledmap_original::Layer const& in, ::tiledmap_original::Layer &out) {
        om.Clone_(in.chunks, out.chunks);
        om.Clone_(in.compression, out.compression);
        om.Clone_(in.data, out.data);
        om.Clone_(in.draworder, out.draworder);
        om.Clone_(in.encoding, out.encoding);
        om.Clone_(in.height, out.height);
        om.Clone_(in.id, out.id);
        om.Clone_(in.image, out.image);
        om.Clone_(in.layers, out.layers);
        om.Clone_(in.name, out.name);
        om.Clone_(in.objects, out.objects);
        om.Clone_(in.offsetx, out.offsetx);
        om.Clone_(in.offsety, out.offsety);
        om.Clone_(in.opacity, out.opacity);
        om.Clone_(in.parallaxx, out.parallaxx);
        om.Clone_(in.parallaxy, out.parallaxy);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.startx, out.startx);
        om.Clone_(in.starty, out.starty);
        om.Clone_(in.tintcolor, out.tintcolor);
        om.Clone_(in.transparentcolor, out.transparentcolor);
        om.Clone_(in.type, out.type);
        om.Clone_(in.visible, out.visible);
        om.Clone_(in.width, out.width);
        om.Clone_(in.x, out.x);
        om.Clone_(in.y, out.y);
    }
    int ObjFuncs<::tiledmap_original::Layer>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Layer const& in) {
        if (int r = om.RecursiveCheck(in.chunks)) return r;
        if (int r = om.RecursiveCheck(in.compression)) return r;
        if (int r = om.RecursiveCheck(in.data)) return r;
        if (int r = om.RecursiveCheck(in.draworder)) return r;
        if (int r = om.RecursiveCheck(in.encoding)) return r;
        if (int r = om.RecursiveCheck(in.height)) return r;
        if (int r = om.RecursiveCheck(in.id)) return r;
        if (int r = om.RecursiveCheck(in.image)) return r;
        if (int r = om.RecursiveCheck(in.layers)) return r;
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.objects)) return r;
        if (int r = om.RecursiveCheck(in.offsetx)) return r;
        if (int r = om.RecursiveCheck(in.offsety)) return r;
        if (int r = om.RecursiveCheck(in.opacity)) return r;
        if (int r = om.RecursiveCheck(in.parallaxx)) return r;
        if (int r = om.RecursiveCheck(in.parallaxy)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.startx)) return r;
        if (int r = om.RecursiveCheck(in.starty)) return r;
        if (int r = om.RecursiveCheck(in.tintcolor)) return r;
        if (int r = om.RecursiveCheck(in.transparentcolor)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        if (int r = om.RecursiveCheck(in.visible)) return r;
        if (int r = om.RecursiveCheck(in.width)) return r;
        if (int r = om.RecursiveCheck(in.x)) return r;
        if (int r = om.RecursiveCheck(in.y)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Layer>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Layer& in) {
        om.RecursiveReset(in.chunks);
        om.RecursiveReset(in.compression);
        om.RecursiveReset(in.data);
        om.RecursiveReset(in.draworder);
        om.RecursiveReset(in.encoding);
        om.RecursiveReset(in.height);
        om.RecursiveReset(in.id);
        om.RecursiveReset(in.image);
        om.RecursiveReset(in.layers);
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.objects);
        om.RecursiveReset(in.offsetx);
        om.RecursiveReset(in.offsety);
        om.RecursiveReset(in.opacity);
        om.RecursiveReset(in.parallaxx);
        om.RecursiveReset(in.parallaxy);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.startx);
        om.RecursiveReset(in.starty);
        om.RecursiveReset(in.tintcolor);
        om.RecursiveReset(in.transparentcolor);
        om.RecursiveReset(in.type);
        om.RecursiveReset(in.visible);
        om.RecursiveReset(in.width);
        om.RecursiveReset(in.x);
        om.RecursiveReset(in.y);
    }
    void ObjFuncs<::tiledmap_original::Layer>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Layer& in) {
        om.SetDefaultValue(in.chunks);
        om.SetDefaultValue(in.compression);
        om.SetDefaultValue(in.data);
        om.SetDefaultValue(in.draworder);
        om.SetDefaultValue(in.encoding);
        in.height = 0;
        in.id = 0;
        om.SetDefaultValue(in.image);
        om.SetDefaultValue(in.layers);
        om.SetDefaultValue(in.name);
        om.SetDefaultValue(in.objects);
        in.offsetx = 0;
        in.offsety = 0;
        in.opacity = 0;
        in.parallaxx = 0;
        in.parallaxy = 0;
        om.SetDefaultValue(in.properties);
        in.startx = 0;
        in.starty = 0;
        om.SetDefaultValue(in.tintcolor);
        om.SetDefaultValue(in.transparentcolor);
        om.SetDefaultValue(in.type);
        in.visible = false;
        in.width = 0;
        in.x = 0;
        in.y = 0;
    }
	void ObjFuncs<::tiledmap_original::Frame, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Frame const& in) {
        d.Write(in.duration);
        d.Write(in.tileid);
    }
	void ObjFuncs<::tiledmap_original::Frame, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Frame const& in) {
        d.Write<false>(in.duration);
        d.Write<false>(in.tileid);
    }
	int ObjFuncs<::tiledmap_original::Frame, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Frame& out) {
        if (int r = d.Read(out.duration)) return r;
        if (int r = d.Read(out.tileid)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Frame, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Frame const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Frame, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Frame const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"duration\":", in.duration); 
        om.Append(s, ",\"tileid\":", in.tileid);
#endif
    }
    void ObjFuncs<::tiledmap_original::Frame>::Clone(::xx::ObjManager& om, ::tiledmap_original::Frame const& in, ::tiledmap_original::Frame &out) {
        om.Clone_(in.duration, out.duration);
        om.Clone_(in.tileid, out.tileid);
    }
    int ObjFuncs<::tiledmap_original::Frame>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Frame const& in) {
        if (int r = om.RecursiveCheck(in.duration)) return r;
        if (int r = om.RecursiveCheck(in.tileid)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Frame>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Frame& in) {
        om.RecursiveReset(in.duration);
        om.RecursiveReset(in.tileid);
    }
    void ObjFuncs<::tiledmap_original::Frame>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Frame& in) {
        in.duration = 0;
        in.tileid = 0;
    }
	void ObjFuncs<::tiledmap_original::WangTile, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangTile const& in) {
        d.Write(in.tileId);
        d.Write(in.wangid);
    }
	void ObjFuncs<::tiledmap_original::WangTile, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangTile const& in) {
        d.Write<false>(in.tileId);
        d.Write<false>(in.wangid);
    }
	int ObjFuncs<::tiledmap_original::WangTile, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::WangTile& out) {
        if (int r = d.Read(out.tileId)) return r;
        if (int r = d.Read(out.wangid)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::WangTile, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::WangTile const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::WangTile, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::WangTile const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"tileId\":", in.tileId); 
        om.Append(s, ",\"wangid\":", in.wangid);
#endif
    }
    void ObjFuncs<::tiledmap_original::WangTile>::Clone(::xx::ObjManager& om, ::tiledmap_original::WangTile const& in, ::tiledmap_original::WangTile &out) {
        om.Clone_(in.tileId, out.tileId);
        om.Clone_(in.wangid, out.wangid);
    }
    int ObjFuncs<::tiledmap_original::WangTile>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::WangTile const& in) {
        if (int r = om.RecursiveCheck(in.tileId)) return r;
        if (int r = om.RecursiveCheck(in.wangid)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::WangTile>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::WangTile& in) {
        om.RecursiveReset(in.tileId);
        om.RecursiveReset(in.wangid);
    }
    void ObjFuncs<::tiledmap_original::WangTile>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::WangTile& in) {
        in.tileId = 0;
        om.SetDefaultValue(in.wangid);
    }
	void ObjFuncs<::tiledmap_original::WangColor, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangColor const& in) {
        d.Write(in.color);
        d.Write(in.name);
        d.Write(in.probability);
        d.Write(in.properties);
        d.Write(in.tile);
    }
	void ObjFuncs<::tiledmap_original::WangColor, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangColor const& in) {
        d.Write<false>(in.color);
        d.Write<false>(in.name);
        d.Write<false>(in.probability);
        d.Write<false>(in.properties);
        d.Write<false>(in.tile);
    }
	int ObjFuncs<::tiledmap_original::WangColor, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::WangColor& out) {
        if (int r = d.Read(out.color)) return r;
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.probability)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.tile)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::WangColor, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::WangColor const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::WangColor, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::WangColor const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"color\":", in.color); 
        om.Append(s, ",\"name\":", in.name);
        om.Append(s, ",\"probability\":", in.probability);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"tile\":", in.tile);
#endif
    }
    void ObjFuncs<::tiledmap_original::WangColor>::Clone(::xx::ObjManager& om, ::tiledmap_original::WangColor const& in, ::tiledmap_original::WangColor &out) {
        om.Clone_(in.color, out.color);
        om.Clone_(in.name, out.name);
        om.Clone_(in.probability, out.probability);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.tile, out.tile);
    }
    int ObjFuncs<::tiledmap_original::WangColor>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::WangColor const& in) {
        if (int r = om.RecursiveCheck(in.color)) return r;
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.probability)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.tile)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::WangColor>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::WangColor& in) {
        om.RecursiveReset(in.color);
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.probability);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.tile);
    }
    void ObjFuncs<::tiledmap_original::WangColor>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::WangColor& in) {
        om.SetDefaultValue(in.color);
        om.SetDefaultValue(in.name);
        in.probability = 0;
        om.SetDefaultValue(in.properties);
        in.tile = 0;
    }
	void ObjFuncs<::tiledmap_original::Grid, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Grid const& in) {
        d.Write(in.height);
        d.Write(in.orientation);
        d.Write(in.width);
    }
	void ObjFuncs<::tiledmap_original::Grid, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Grid const& in) {
        d.Write<false>(in.height);
        d.Write<false>(in.orientation);
        d.Write<false>(in.width);
    }
	int ObjFuncs<::tiledmap_original::Grid, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Grid& out) {
        if (int r = d.Read(out.height)) return r;
        if (int r = d.Read(out.orientation)) return r;
        if (int r = d.Read(out.width)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Grid, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Grid const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Grid, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Grid const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"height\":", in.height); 
        om.Append(s, ",\"orientation\":", in.orientation);
        om.Append(s, ",\"width\":", in.width);
#endif
    }
    void ObjFuncs<::tiledmap_original::Grid>::Clone(::xx::ObjManager& om, ::tiledmap_original::Grid const& in, ::tiledmap_original::Grid &out) {
        om.Clone_(in.height, out.height);
        om.Clone_(in.orientation, out.orientation);
        om.Clone_(in.width, out.width);
    }
    int ObjFuncs<::tiledmap_original::Grid>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Grid const& in) {
        if (int r = om.RecursiveCheck(in.height)) return r;
        if (int r = om.RecursiveCheck(in.orientation)) return r;
        if (int r = om.RecursiveCheck(in.width)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Grid>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Grid& in) {
        om.RecursiveReset(in.height);
        om.RecursiveReset(in.orientation);
        om.RecursiveReset(in.width);
    }
    void ObjFuncs<::tiledmap_original::Grid>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Grid& in) {
        in.height = 0;
        om.SetDefaultValue(in.orientation);
        in.width = 0;
    }
	void ObjFuncs<::tiledmap_original::Terrain, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Terrain const& in) {
        d.Write(in.name);
        d.Write(in.properties);
        d.Write(in.tile);
    }
	void ObjFuncs<::tiledmap_original::Terrain, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Terrain const& in) {
        d.Write<false>(in.name);
        d.Write<false>(in.properties);
        d.Write<false>(in.tile);
    }
	int ObjFuncs<::tiledmap_original::Terrain, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Terrain& out) {
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.tile)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Terrain, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Terrain const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Terrain, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Terrain const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"name\":", in.name); 
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"tile\":", in.tile);
#endif
    }
    void ObjFuncs<::tiledmap_original::Terrain>::Clone(::xx::ObjManager& om, ::tiledmap_original::Terrain const& in, ::tiledmap_original::Terrain &out) {
        om.Clone_(in.name, out.name);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.tile, out.tile);
    }
    int ObjFuncs<::tiledmap_original::Terrain>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Terrain const& in) {
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.tile)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Terrain>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Terrain& in) {
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.tile);
    }
    void ObjFuncs<::tiledmap_original::Terrain>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Terrain& in) {
        om.SetDefaultValue(in.name);
        om.SetDefaultValue(in.properties);
        in.tile = 0;
    }
	void ObjFuncs<::tiledmap_original::TileOffset, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::TileOffset const& in) {
        d.Write(in.x);
        d.Write(in.y);
    }
	void ObjFuncs<::tiledmap_original::TileOffset, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::TileOffset const& in) {
        d.Write<false>(in.x);
        d.Write<false>(in.y);
    }
	int ObjFuncs<::tiledmap_original::TileOffset, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::TileOffset& out) {
        if (int r = d.Read(out.x)) return r;
        if (int r = d.Read(out.y)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::TileOffset, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::TileOffset const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::TileOffset, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::TileOffset const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"x\":", in.x); 
        om.Append(s, ",\"y\":", in.y);
#endif
    }
    void ObjFuncs<::tiledmap_original::TileOffset>::Clone(::xx::ObjManager& om, ::tiledmap_original::TileOffset const& in, ::tiledmap_original::TileOffset &out) {
        om.Clone_(in.x, out.x);
        om.Clone_(in.y, out.y);
    }
    int ObjFuncs<::tiledmap_original::TileOffset>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::TileOffset const& in) {
        if (int r = om.RecursiveCheck(in.x)) return r;
        if (int r = om.RecursiveCheck(in.y)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::TileOffset>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::TileOffset& in) {
        om.RecursiveReset(in.x);
        om.RecursiveReset(in.y);
    }
    void ObjFuncs<::tiledmap_original::TileOffset>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::TileOffset& in) {
        in.x = 0;
        in.y = 0;
    }
	void ObjFuncs<::tiledmap_original::Tile, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Tile const& in) {
        d.Write(in.animation);
        d.Write(in.id);
        d.Write(in.image);
        d.Write(in.imageheight);
        d.Write(in.imagewidth);
        d.Write(in.objectgroup);
        d.Write(in.probability);
        d.Write(in.properties);
        d.Write(in.terrain);
        d.Write(in.type);
    }
	void ObjFuncs<::tiledmap_original::Tile, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Tile const& in) {
        d.Write<false>(in.animation);
        d.Write<false>(in.id);
        d.Write<false>(in.image);
        d.Write<false>(in.imageheight);
        d.Write<false>(in.imagewidth);
        d.Write<false>(in.objectgroup);
        d.Write<false>(in.probability);
        d.Write<false>(in.properties);
        d.Write<false>(in.terrain);
        d.Write<false>(in.type);
    }
	int ObjFuncs<::tiledmap_original::Tile, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Tile& out) {
        if (int r = d.Read(out.animation)) return r;
        if (int r = d.Read(out.id)) return r;
        if (int r = d.Read(out.image)) return r;
        if (int r = d.Read(out.imageheight)) return r;
        if (int r = d.Read(out.imagewidth)) return r;
        if (int r = d.Read(out.objectgroup)) return r;
        if (int r = d.Read(out.probability)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.terrain)) return r;
        if (int r = d.Read(out.type)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Tile, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Tile const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Tile, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Tile const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"animation\":", in.animation); 
        om.Append(s, ",\"id\":", in.id);
        om.Append(s, ",\"image\":", in.image);
        om.Append(s, ",\"imageheight\":", in.imageheight);
        om.Append(s, ",\"imagewidth\":", in.imagewidth);
        om.Append(s, ",\"objectgroup\":", in.objectgroup);
        om.Append(s, ",\"probability\":", in.probability);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"terrain\":", in.terrain);
        om.Append(s, ",\"type\":", in.type);
#endif
    }
    void ObjFuncs<::tiledmap_original::Tile>::Clone(::xx::ObjManager& om, ::tiledmap_original::Tile const& in, ::tiledmap_original::Tile &out) {
        om.Clone_(in.animation, out.animation);
        om.Clone_(in.id, out.id);
        om.Clone_(in.image, out.image);
        om.Clone_(in.imageheight, out.imageheight);
        om.Clone_(in.imagewidth, out.imagewidth);
        om.Clone_(in.objectgroup, out.objectgroup);
        om.Clone_(in.probability, out.probability);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.terrain, out.terrain);
        om.Clone_(in.type, out.type);
    }
    int ObjFuncs<::tiledmap_original::Tile>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Tile const& in) {
        if (int r = om.RecursiveCheck(in.animation)) return r;
        if (int r = om.RecursiveCheck(in.id)) return r;
        if (int r = om.RecursiveCheck(in.image)) return r;
        if (int r = om.RecursiveCheck(in.imageheight)) return r;
        if (int r = om.RecursiveCheck(in.imagewidth)) return r;
        if (int r = om.RecursiveCheck(in.objectgroup)) return r;
        if (int r = om.RecursiveCheck(in.probability)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.terrain)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Tile>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Tile& in) {
        om.RecursiveReset(in.animation);
        om.RecursiveReset(in.id);
        om.RecursiveReset(in.image);
        om.RecursiveReset(in.imageheight);
        om.RecursiveReset(in.imagewidth);
        om.RecursiveReset(in.objectgroup);
        om.RecursiveReset(in.probability);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.terrain);
        om.RecursiveReset(in.type);
    }
    void ObjFuncs<::tiledmap_original::Tile>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Tile& in) {
        om.SetDefaultValue(in.animation);
        in.id = 0;
        om.SetDefaultValue(in.image);
        in.imageheight = 0;
        in.imagewidth = 0;
        om.SetDefaultValue(in.objectgroup);
        in.probability = 0;
        om.SetDefaultValue(in.properties);
        om.SetDefaultValue(in.terrain);
        om.SetDefaultValue(in.type);
    }
	void ObjFuncs<::tiledmap_original::Transformations, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Transformations const& in) {
        d.Write(in.hflip);
        d.Write(in.vflip);
        d.Write(in.rotate);
        d.Write(in.preferuntransformed);
    }
	void ObjFuncs<::tiledmap_original::Transformations, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Transformations const& in) {
        d.Write<false>(in.hflip);
        d.Write<false>(in.vflip);
        d.Write<false>(in.rotate);
        d.Write<false>(in.preferuntransformed);
    }
	int ObjFuncs<::tiledmap_original::Transformations, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Transformations& out) {
        if (int r = d.Read(out.hflip)) return r;
        if (int r = d.Read(out.vflip)) return r;
        if (int r = d.Read(out.rotate)) return r;
        if (int r = d.Read(out.preferuntransformed)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Transformations, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Transformations const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Transformations, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Transformations const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"hflip\":", in.hflip); 
        om.Append(s, ",\"vflip\":", in.vflip);
        om.Append(s, ",\"rotate\":", in.rotate);
        om.Append(s, ",\"preferuntransformed\":", in.preferuntransformed);
#endif
    }
    void ObjFuncs<::tiledmap_original::Transformations>::Clone(::xx::ObjManager& om, ::tiledmap_original::Transformations const& in, ::tiledmap_original::Transformations &out) {
        om.Clone_(in.hflip, out.hflip);
        om.Clone_(in.vflip, out.vflip);
        om.Clone_(in.rotate, out.rotate);
        om.Clone_(in.preferuntransformed, out.preferuntransformed);
    }
    int ObjFuncs<::tiledmap_original::Transformations>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Transformations const& in) {
        if (int r = om.RecursiveCheck(in.hflip)) return r;
        if (int r = om.RecursiveCheck(in.vflip)) return r;
        if (int r = om.RecursiveCheck(in.rotate)) return r;
        if (int r = om.RecursiveCheck(in.preferuntransformed)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Transformations>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Transformations& in) {
        om.RecursiveReset(in.hflip);
        om.RecursiveReset(in.vflip);
        om.RecursiveReset(in.rotate);
        om.RecursiveReset(in.preferuntransformed);
    }
    void ObjFuncs<::tiledmap_original::Transformations>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Transformations& in) {
        in.hflip = false;
        in.vflip = false;
        in.rotate = false;
        in.preferuntransformed = false;
    }
	void ObjFuncs<::tiledmap_original::WangSet, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangSet const& in) {
        d.Write(in.colors);
        d.Write(in.name);
        d.Write(in.properties);
        d.Write(in.tile);
        d.Write(in.wangtiles);
    }
	void ObjFuncs<::tiledmap_original::WangSet, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::WangSet const& in) {
        d.Write<false>(in.colors);
        d.Write<false>(in.name);
        d.Write<false>(in.properties);
        d.Write<false>(in.tile);
        d.Write<false>(in.wangtiles);
    }
	int ObjFuncs<::tiledmap_original::WangSet, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::WangSet& out) {
        if (int r = d.Read(out.colors)) return r;
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.tile)) return r;
        if (int r = d.Read(out.wangtiles)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::WangSet, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::WangSet const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::WangSet, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::WangSet const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"colors\":", in.colors); 
        om.Append(s, ",\"name\":", in.name);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"tile\":", in.tile);
        om.Append(s, ",\"wangtiles\":", in.wangtiles);
#endif
    }
    void ObjFuncs<::tiledmap_original::WangSet>::Clone(::xx::ObjManager& om, ::tiledmap_original::WangSet const& in, ::tiledmap_original::WangSet &out) {
        om.Clone_(in.colors, out.colors);
        om.Clone_(in.name, out.name);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.tile, out.tile);
        om.Clone_(in.wangtiles, out.wangtiles);
    }
    int ObjFuncs<::tiledmap_original::WangSet>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::WangSet const& in) {
        if (int r = om.RecursiveCheck(in.colors)) return r;
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.tile)) return r;
        if (int r = om.RecursiveCheck(in.wangtiles)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::WangSet>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::WangSet& in) {
        om.RecursiveReset(in.colors);
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.tile);
        om.RecursiveReset(in.wangtiles);
    }
    void ObjFuncs<::tiledmap_original::WangSet>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::WangSet& in) {
        om.SetDefaultValue(in.colors);
        om.SetDefaultValue(in.name);
        om.SetDefaultValue(in.properties);
        in.tile = 0;
        om.SetDefaultValue(in.wangtiles);
    }
	void ObjFuncs<::tiledmap_original::Tileset, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Tileset const& in) {
        d.Write(in.backgroundcolor);
        d.Write(in.columns);
        d.Write(in.firstgid);
        d.Write(in.grid);
        d.Write(in.image);
        d.Write(in.imageheight);
        d.Write(in.imagewidth);
        d.Write(in.margin);
        d.Write(in.name);
        d.Write(in.objectalignment);
        d.Write(in.properties);
        d.Write(in.source);
        d.Write(in.spacing);
        d.Write(in.terrains);
        d.Write(in.tilecount);
        d.Write(in.tiledversion);
        d.Write(in.tileheight);
        d.Write(in.tileoffset);
        d.Write(in.tiles);
        d.Write(in.tilewidth);
        d.Write(in.transformations);
        d.Write(in.transparentcolor);
        d.Write(in.type);
        d.Write(in.version);
        d.Write(in.wangsets);
    }
	void ObjFuncs<::tiledmap_original::Tileset, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Tileset const& in) {
        d.Write<false>(in.backgroundcolor);
        d.Write<false>(in.columns);
        d.Write<false>(in.firstgid);
        d.Write<false>(in.grid);
        d.Write<false>(in.image);
        d.Write<false>(in.imageheight);
        d.Write<false>(in.imagewidth);
        d.Write<false>(in.margin);
        d.Write<false>(in.name);
        d.Write<false>(in.objectalignment);
        d.Write<false>(in.properties);
        d.Write<false>(in.source);
        d.Write<false>(in.spacing);
        d.Write<false>(in.terrains);
        d.Write<false>(in.tilecount);
        d.Write<false>(in.tiledversion);
        d.Write<false>(in.tileheight);
        d.Write<false>(in.tileoffset);
        d.Write<false>(in.tiles);
        d.Write<false>(in.tilewidth);
        d.Write<false>(in.transformations);
        d.Write<false>(in.transparentcolor);
        d.Write<false>(in.type);
        d.Write<false>(in.version);
        d.Write<false>(in.wangsets);
    }
	int ObjFuncs<::tiledmap_original::Tileset, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Tileset& out) {
        if (int r = d.Read(out.backgroundcolor)) return r;
        if (int r = d.Read(out.columns)) return r;
        if (int r = d.Read(out.firstgid)) return r;
        if (int r = d.Read(out.grid)) return r;
        if (int r = d.Read(out.image)) return r;
        if (int r = d.Read(out.imageheight)) return r;
        if (int r = d.Read(out.imagewidth)) return r;
        if (int r = d.Read(out.margin)) return r;
        if (int r = d.Read(out.name)) return r;
        if (int r = d.Read(out.objectalignment)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.source)) return r;
        if (int r = d.Read(out.spacing)) return r;
        if (int r = d.Read(out.terrains)) return r;
        if (int r = d.Read(out.tilecount)) return r;
        if (int r = d.Read(out.tiledversion)) return r;
        if (int r = d.Read(out.tileheight)) return r;
        if (int r = d.Read(out.tileoffset)) return r;
        if (int r = d.Read(out.tiles)) return r;
        if (int r = d.Read(out.tilewidth)) return r;
        if (int r = d.Read(out.transformations)) return r;
        if (int r = d.Read(out.transparentcolor)) return r;
        if (int r = d.Read(out.type)) return r;
        if (int r = d.Read(out.version)) return r;
        if (int r = d.Read(out.wangsets)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Tileset, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Tileset const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Tileset, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Tileset const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"backgroundcolor\":", in.backgroundcolor); 
        om.Append(s, ",\"columns\":", in.columns);
        om.Append(s, ",\"firstgid\":", in.firstgid);
        om.Append(s, ",\"grid\":", in.grid);
        om.Append(s, ",\"image\":", in.image);
        om.Append(s, ",\"imageheight\":", in.imageheight);
        om.Append(s, ",\"imagewidth\":", in.imagewidth);
        om.Append(s, ",\"margin\":", in.margin);
        om.Append(s, ",\"name\":", in.name);
        om.Append(s, ",\"objectalignment\":", in.objectalignment);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"source\":", in.source);
        om.Append(s, ",\"spacing\":", in.spacing);
        om.Append(s, ",\"terrains\":", in.terrains);
        om.Append(s, ",\"tilecount\":", in.tilecount);
        om.Append(s, ",\"tiledversion\":", in.tiledversion);
        om.Append(s, ",\"tileheight\":", in.tileheight);
        om.Append(s, ",\"tileoffset\":", in.tileoffset);
        om.Append(s, ",\"tiles\":", in.tiles);
        om.Append(s, ",\"tilewidth\":", in.tilewidth);
        om.Append(s, ",\"transformations\":", in.transformations);
        om.Append(s, ",\"transparentcolor\":", in.transparentcolor);
        om.Append(s, ",\"type\":", in.type);
        om.Append(s, ",\"version\":", in.version);
        om.Append(s, ",\"wangsets\":", in.wangsets);
#endif
    }
    void ObjFuncs<::tiledmap_original::Tileset>::Clone(::xx::ObjManager& om, ::tiledmap_original::Tileset const& in, ::tiledmap_original::Tileset &out) {
        om.Clone_(in.backgroundcolor, out.backgroundcolor);
        om.Clone_(in.columns, out.columns);
        om.Clone_(in.firstgid, out.firstgid);
        om.Clone_(in.grid, out.grid);
        om.Clone_(in.image, out.image);
        om.Clone_(in.imageheight, out.imageheight);
        om.Clone_(in.imagewidth, out.imagewidth);
        om.Clone_(in.margin, out.margin);
        om.Clone_(in.name, out.name);
        om.Clone_(in.objectalignment, out.objectalignment);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.source, out.source);
        om.Clone_(in.spacing, out.spacing);
        om.Clone_(in.terrains, out.terrains);
        om.Clone_(in.tilecount, out.tilecount);
        om.Clone_(in.tiledversion, out.tiledversion);
        om.Clone_(in.tileheight, out.tileheight);
        om.Clone_(in.tileoffset, out.tileoffset);
        om.Clone_(in.tiles, out.tiles);
        om.Clone_(in.tilewidth, out.tilewidth);
        om.Clone_(in.transformations, out.transformations);
        om.Clone_(in.transparentcolor, out.transparentcolor);
        om.Clone_(in.type, out.type);
        om.Clone_(in.version, out.version);
        om.Clone_(in.wangsets, out.wangsets);
    }
    int ObjFuncs<::tiledmap_original::Tileset>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Tileset const& in) {
        if (int r = om.RecursiveCheck(in.backgroundcolor)) return r;
        if (int r = om.RecursiveCheck(in.columns)) return r;
        if (int r = om.RecursiveCheck(in.firstgid)) return r;
        if (int r = om.RecursiveCheck(in.grid)) return r;
        if (int r = om.RecursiveCheck(in.image)) return r;
        if (int r = om.RecursiveCheck(in.imageheight)) return r;
        if (int r = om.RecursiveCheck(in.imagewidth)) return r;
        if (int r = om.RecursiveCheck(in.margin)) return r;
        if (int r = om.RecursiveCheck(in.name)) return r;
        if (int r = om.RecursiveCheck(in.objectalignment)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.source)) return r;
        if (int r = om.RecursiveCheck(in.spacing)) return r;
        if (int r = om.RecursiveCheck(in.terrains)) return r;
        if (int r = om.RecursiveCheck(in.tilecount)) return r;
        if (int r = om.RecursiveCheck(in.tiledversion)) return r;
        if (int r = om.RecursiveCheck(in.tileheight)) return r;
        if (int r = om.RecursiveCheck(in.tileoffset)) return r;
        if (int r = om.RecursiveCheck(in.tiles)) return r;
        if (int r = om.RecursiveCheck(in.tilewidth)) return r;
        if (int r = om.RecursiveCheck(in.transformations)) return r;
        if (int r = om.RecursiveCheck(in.transparentcolor)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        if (int r = om.RecursiveCheck(in.version)) return r;
        if (int r = om.RecursiveCheck(in.wangsets)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Tileset>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Tileset& in) {
        om.RecursiveReset(in.backgroundcolor);
        om.RecursiveReset(in.columns);
        om.RecursiveReset(in.firstgid);
        om.RecursiveReset(in.grid);
        om.RecursiveReset(in.image);
        om.RecursiveReset(in.imageheight);
        om.RecursiveReset(in.imagewidth);
        om.RecursiveReset(in.margin);
        om.RecursiveReset(in.name);
        om.RecursiveReset(in.objectalignment);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.source);
        om.RecursiveReset(in.spacing);
        om.RecursiveReset(in.terrains);
        om.RecursiveReset(in.tilecount);
        om.RecursiveReset(in.tiledversion);
        om.RecursiveReset(in.tileheight);
        om.RecursiveReset(in.tileoffset);
        om.RecursiveReset(in.tiles);
        om.RecursiveReset(in.tilewidth);
        om.RecursiveReset(in.transformations);
        om.RecursiveReset(in.transparentcolor);
        om.RecursiveReset(in.type);
        om.RecursiveReset(in.version);
        om.RecursiveReset(in.wangsets);
    }
    void ObjFuncs<::tiledmap_original::Tileset>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Tileset& in) {
        om.SetDefaultValue(in.backgroundcolor);
        in.columns = 0;
        in.firstgid = 0;
        om.SetDefaultValue(in.grid);
        om.SetDefaultValue(in.image);
        in.imageheight = 0;
        in.imagewidth = 0;
        in.margin = 0;
        om.SetDefaultValue(in.name);
        om.SetDefaultValue(in.objectalignment);
        om.SetDefaultValue(in.properties);
        om.SetDefaultValue(in.source);
        in.spacing = 0;
        om.SetDefaultValue(in.terrains);
        in.tilecount = 0;
        om.SetDefaultValue(in.tiledversion);
        in.tileheight = 0;
        om.SetDefaultValue(in.tileoffset);
        om.SetDefaultValue(in.tiles);
        in.tilewidth = 0;
        om.SetDefaultValue(in.transformations);
        om.SetDefaultValue(in.transparentcolor);
        om.SetDefaultValue(in.type);
        om.SetDefaultValue(in.version);
        om.SetDefaultValue(in.wangsets);
    }
	void ObjFuncs<::tiledmap_original::Map, void>::Write(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Map const& in) {
        d.Write(in.backgroundcolor);
        d.Write(in.compressionlevel);
        d.Write(in.height);
        d.Write(in.hexsidelength);
        d.Write(in.infinite);
        d.Write(in.layers);
        d.Write(in.nextlayerid);
        d.Write(in.nextobjectid);
        d.Write(in.orientation);
        d.Write(in.properties);
        d.Write(in.renderorder);
        d.Write(in.staggeraxis);
        d.Write(in.staggerindex);
        d.Write(in.tiledversion);
        d.Write(in.tileheight);
        d.Write(in.tilesets);
        d.Write(in.tilewidth);
        d.Write(in.type);
        d.Write(in.version);
        d.Write(in.width);
    }
	void ObjFuncs<::tiledmap_original::Map, void>::WriteFast(::xx::ObjManager& om, ::xx::Data& d, ::tiledmap_original::Map const& in) {
        d.Write<false>(in.backgroundcolor);
        d.Write<false>(in.compressionlevel);
        d.Write<false>(in.height);
        d.Write<false>(in.hexsidelength);
        d.Write<false>(in.infinite);
        d.Write<false>(in.layers);
        d.Write<false>(in.nextlayerid);
        d.Write<false>(in.nextobjectid);
        d.Write<false>(in.orientation);
        d.Write<false>(in.properties);
        d.Write<false>(in.renderorder);
        d.Write<false>(in.staggeraxis);
        d.Write<false>(in.staggerindex);
        d.Write<false>(in.tiledversion);
        d.Write<false>(in.tileheight);
        d.Write<false>(in.tilesets);
        d.Write<false>(in.tilewidth);
        d.Write<false>(in.type);
        d.Write<false>(in.version);
        d.Write<false>(in.width);
    }
	int ObjFuncs<::tiledmap_original::Map, void>::Read(::xx::ObjManager& om, ::xx::Data_r& d, ::tiledmap_original::Map& out) {
        if (int r = d.Read(out.backgroundcolor)) return r;
        if (int r = d.Read(out.compressionlevel)) return r;
        if (int r = d.Read(out.height)) return r;
        if (int r = d.Read(out.hexsidelength)) return r;
        if (int r = d.Read(out.infinite)) return r;
        if (int r = d.Read(out.layers)) return r;
        if (int r = d.Read(out.nextlayerid)) return r;
        if (int r = d.Read(out.nextobjectid)) return r;
        if (int r = d.Read(out.orientation)) return r;
        if (int r = d.Read(out.properties)) return r;
        if (int r = d.Read(out.renderorder)) return r;
        if (int r = d.Read(out.staggeraxis)) return r;
        if (int r = d.Read(out.staggerindex)) return r;
        if (int r = d.Read(out.tiledversion)) return r;
        if (int r = d.Read(out.tileheight)) return r;
        if (int r = d.Read(out.tilesets)) return r;
        if (int r = d.Read(out.tilewidth)) return r;
        if (int r = d.Read(out.type)) return r;
        if (int r = d.Read(out.version)) return r;
        if (int r = d.Read(out.width)) return r;
        return 0;
    }
	void ObjFuncs<::tiledmap_original::Map, void>::Append(ObjManager &om, std::string& s, ::tiledmap_original::Map const& in) {
#ifndef XX_DISABLE_APPEND
        s.push_back('{');
        AppendCore(om, s, in);
        s.push_back('}');
#endif
    }
	void ObjFuncs<::tiledmap_original::Map, void>::AppendCore(ObjManager &om, std::string& s, ::tiledmap_original::Map const& in) {
#ifndef XX_DISABLE_APPEND
        om.Append(s, "\"backgroundcolor\":", in.backgroundcolor); 
        om.Append(s, ",\"compressionlevel\":", in.compressionlevel);
        om.Append(s, ",\"height\":", in.height);
        om.Append(s, ",\"hexsidelength\":", in.hexsidelength);
        om.Append(s, ",\"infinite\":", in.infinite);
        om.Append(s, ",\"layers\":", in.layers);
        om.Append(s, ",\"nextlayerid\":", in.nextlayerid);
        om.Append(s, ",\"nextobjectid\":", in.nextobjectid);
        om.Append(s, ",\"orientation\":", in.orientation);
        om.Append(s, ",\"properties\":", in.properties);
        om.Append(s, ",\"renderorder\":", in.renderorder);
        om.Append(s, ",\"staggeraxis\":", in.staggeraxis);
        om.Append(s, ",\"staggerindex\":", in.staggerindex);
        om.Append(s, ",\"tiledversion\":", in.tiledversion);
        om.Append(s, ",\"tileheight\":", in.tileheight);
        om.Append(s, ",\"tilesets\":", in.tilesets);
        om.Append(s, ",\"tilewidth\":", in.tilewidth);
        om.Append(s, ",\"type\":", in.type);
        om.Append(s, ",\"version\":", in.version);
        om.Append(s, ",\"width\":", in.width);
#endif
    }
    void ObjFuncs<::tiledmap_original::Map>::Clone(::xx::ObjManager& om, ::tiledmap_original::Map const& in, ::tiledmap_original::Map &out) {
        om.Clone_(in.backgroundcolor, out.backgroundcolor);
        om.Clone_(in.compressionlevel, out.compressionlevel);
        om.Clone_(in.height, out.height);
        om.Clone_(in.hexsidelength, out.hexsidelength);
        om.Clone_(in.infinite, out.infinite);
        om.Clone_(in.layers, out.layers);
        om.Clone_(in.nextlayerid, out.nextlayerid);
        om.Clone_(in.nextobjectid, out.nextobjectid);
        om.Clone_(in.orientation, out.orientation);
        om.Clone_(in.properties, out.properties);
        om.Clone_(in.renderorder, out.renderorder);
        om.Clone_(in.staggeraxis, out.staggeraxis);
        om.Clone_(in.staggerindex, out.staggerindex);
        om.Clone_(in.tiledversion, out.tiledversion);
        om.Clone_(in.tileheight, out.tileheight);
        om.Clone_(in.tilesets, out.tilesets);
        om.Clone_(in.tilewidth, out.tilewidth);
        om.Clone_(in.type, out.type);
        om.Clone_(in.version, out.version);
        om.Clone_(in.width, out.width);
    }
    int ObjFuncs<::tiledmap_original::Map>::RecursiveCheck(::xx::ObjManager& om, ::tiledmap_original::Map const& in) {
        if (int r = om.RecursiveCheck(in.backgroundcolor)) return r;
        if (int r = om.RecursiveCheck(in.compressionlevel)) return r;
        if (int r = om.RecursiveCheck(in.height)) return r;
        if (int r = om.RecursiveCheck(in.hexsidelength)) return r;
        if (int r = om.RecursiveCheck(in.infinite)) return r;
        if (int r = om.RecursiveCheck(in.layers)) return r;
        if (int r = om.RecursiveCheck(in.nextlayerid)) return r;
        if (int r = om.RecursiveCheck(in.nextobjectid)) return r;
        if (int r = om.RecursiveCheck(in.orientation)) return r;
        if (int r = om.RecursiveCheck(in.properties)) return r;
        if (int r = om.RecursiveCheck(in.renderorder)) return r;
        if (int r = om.RecursiveCheck(in.staggeraxis)) return r;
        if (int r = om.RecursiveCheck(in.staggerindex)) return r;
        if (int r = om.RecursiveCheck(in.tiledversion)) return r;
        if (int r = om.RecursiveCheck(in.tileheight)) return r;
        if (int r = om.RecursiveCheck(in.tilesets)) return r;
        if (int r = om.RecursiveCheck(in.tilewidth)) return r;
        if (int r = om.RecursiveCheck(in.type)) return r;
        if (int r = om.RecursiveCheck(in.version)) return r;
        if (int r = om.RecursiveCheck(in.width)) return r;
        return 0;
    }
    void ObjFuncs<::tiledmap_original::Map>::RecursiveReset(::xx::ObjManager& om, ::tiledmap_original::Map& in) {
        om.RecursiveReset(in.backgroundcolor);
        om.RecursiveReset(in.compressionlevel);
        om.RecursiveReset(in.height);
        om.RecursiveReset(in.hexsidelength);
        om.RecursiveReset(in.infinite);
        om.RecursiveReset(in.layers);
        om.RecursiveReset(in.nextlayerid);
        om.RecursiveReset(in.nextobjectid);
        om.RecursiveReset(in.orientation);
        om.RecursiveReset(in.properties);
        om.RecursiveReset(in.renderorder);
        om.RecursiveReset(in.staggeraxis);
        om.RecursiveReset(in.staggerindex);
        om.RecursiveReset(in.tiledversion);
        om.RecursiveReset(in.tileheight);
        om.RecursiveReset(in.tilesets);
        om.RecursiveReset(in.tilewidth);
        om.RecursiveReset(in.type);
        om.RecursiveReset(in.version);
        om.RecursiveReset(in.width);
    }
    void ObjFuncs<::tiledmap_original::Map>::SetDefaultValue(::xx::ObjManager& om, ::tiledmap_original::Map& in) {
        om.SetDefaultValue(in.backgroundcolor);
        in.compressionlevel = 0;
        in.height = 0;
        in.hexsidelength = 0;
        in.infinite = false;
        om.SetDefaultValue(in.layers);
        in.nextlayerid = 0;
        in.nextobjectid = 0;
        om.SetDefaultValue(in.orientation);
        om.SetDefaultValue(in.properties);
        om.SetDefaultValue(in.renderorder);
        om.SetDefaultValue(in.staggeraxis);
        om.SetDefaultValue(in.staggerindex);
        om.SetDefaultValue(in.tiledversion);
        in.tileheight = 0;
        om.SetDefaultValue(in.tilesets);
        in.tilewidth = 0;
        om.SetDefaultValue(in.type);
        om.SetDefaultValue(in.version);
        in.width = 0;
    }
}
