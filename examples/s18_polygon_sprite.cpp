#include "xx2d_pch.h"
#include "game_looper.h"
#include "s18_polygon_sprite.h"

namespace PolygonSprite {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "PolygonSprite::Scene::Init" << std::endl;

		xx::TP tp;
		tp.Fill("res/tree.plist");
		for (auto& f : tp.frames) {
			auto xys = (xx::XY*)f->vertices.data();
			auto uvs = (xx::XY*)f->verticesUV.data();
			vs.resize(f->verticesUV.size() / 2);
			for (size_t i = 0, e = vs.size(); i < e; i++) {
				vs[i].x = xys[i].x;
				vs[i].y = xys[i].y;
				vs[i].u = uvs[i].x;
				vs[i].v = uvs[i].y;
			}
			//for (size_t i = 0, e = vs.size(); i < e; i++) {
			//	(xx::XY&)vs[i].x = at.Apply(xys[i]);
			//}
			xx::RGBA8 color{ 255,255,255,255 };
			for (auto& v : vs) {
				memcpy(&v.r, &color, sizeof(color));
			}
			for (auto& i : f->triangles) {
				is.push_back(i);
			}
			tex = f->tex;
			break;
		}
	}

	int Scene::Update() {
		auto [offset, pv, pi] = xx::engine.sm.GetShader<xx::Shader_Verts>().Draw(*tex, vs.size(), is.size());
		memcpy(pv, vs.data(), sizeof(typename decltype(vs)::value_type) * vs.size());
		for (size_t i = 0; i < is.size(); i++) {
			pi[i] = offset + is[i];
		}
		return 0;
	}
}
