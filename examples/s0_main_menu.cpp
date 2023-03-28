#include "main.h"
#include "s0_main_menu.h"
#include "s1_tiledmap.h"
#include "s2_dragable.h"
#include "s3_boxbutton.h"
#include "s4_physics_circles.h"
#include "s5_spacegridab.h"
#include "s6_box_circle_cd.h"
#include "s7_more_box_circle_cd.h"
#include "s8_node.h"
#include "s9_sprites.h"
#include "s10_quads.h"
#include "s11_shootgame.h"
#include "s12_shootgame_idx.h"
#include "s13_movepath.h"
#include "s14_scissor.h"
#include "s15_rendertexture.h"
#include "s16_audio.h"
#include "s17_imgui.h"
#include "s18_polygon_sprite.h"
#include "s19_space_shooter.h"
#include "s20_xxmv.h"
#include "s21_particle.h"

namespace MainMenu {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "MainMenu::Scene::Init" << std::endl;

		meListener.Init(xx::Mbtns::Left);

		float x = 0, xstep = xx::engine.w / 3 - 50;
		float y = 300, yinc = 100;
		menus.emplace_back().Init(looper, { -xstep, y }, "1: tiledmap", 32);
		menus.emplace_back().Init(looper, { 0, y }, "2: dragable", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "3: box button", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "4: physics circles (space grid) ", 32);
		menus.emplace_back().Init(looper, { 0, y }, "5: boxs (space grid ab)", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "6: box & circle physics", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "7: more circle + box physics", 32);
		menus.emplace_back().Init(looper, { 0, y }, "8: node", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "9: sprites", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "10: quads", 32);
		menus.emplace_back().Init(looper, { 0, y }, "11: shooter game (slowly)", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "12: shooter game with index", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "13: move path", 32);
		menus.emplace_back().Init(looper, { 0, y }, "14: scissor", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "15: render texture", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "16: audio", 32);
		menus.emplace_back().Init(looper, { 0, y }, "17: imgui", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "18: polygon sprite", 32);

		y -= yinc;
		menus.emplace_back().Init(looper, { -xstep, y }, "19: space shooter", 32);
		menus.emplace_back().Init(looper, { 0, y }, "20: particle", 32);
		menus.emplace_back().Init(looper, { xstep, y }, "21: xxmv (vp9 webm)", 32);

		looper->fpsViewer.extraInfo.clear();
	}

	int Scene::Update() {

		// handle mouse event
		meListener.Update();
		auto&& iter = menus.begin();
		while (meListener.eventId && iter != menus.end()) {
			meListener.Dispatch(&*iter++);
		}

		for (auto&& m : menus) {
			m.content.Draw();
		}

		return 0;
	}

	bool Menu::HandleMouseDown(MenuMouseEventListener& L) {
		return Inside(L.downPos);
	}

	int Menu::HandleMouseMove(MenuMouseEventListener& L) {
		return 0;
	}

	void Menu::HandleMouseUp(MenuMouseEventListener& L) {
		if (Inside(xx::engine.mousePosition)) {
			if (txt.starts_with("1:"sv)) {
				looper->DelaySwitchTo<TiledMap::Scene>();
			} else if (txt.starts_with("2:"sv)) {
				looper->DelaySwitchTo<Dragable::Scene>();
			} else if (txt.starts_with("3:"sv)) {
				looper->DelaySwitchTo<BoxButton::Scene>();
			} else if (txt.starts_with("4:"sv)) {
				looper->DelaySwitchTo<PhysicsCircles::Scene>();
			} else if (txt.starts_with("5:"sv)) {
				looper->DelaySwitchTo<SpaceGridAB::Scene>();
			} else if (txt.starts_with("6:"sv)) {
				looper->DelaySwitchTo<BoxCircleCD::Scene>();
			} else if (txt.starts_with("7:"sv)) {
				looper->DelaySwitchTo<MoreBoxCircleCD::Scene>();
			} else if (txt.starts_with("8:"sv)) {
				looper->DelaySwitchTo<Node::Scene>();
			} else if (txt.starts_with("9:"sv)) {
				looper->DelaySwitchTo<Sprites::Scene>();
			} else if (txt.starts_with("10:"sv)) {
				looper->DelaySwitchTo<Quads::Scene>();
			} else if (txt.starts_with("11:"sv)) {
				looper->DelaySwitchTo<ShootGame::Looper>();
			} else if (txt.starts_with("12:"sv)) {
				looper->DelaySwitchTo<ShootGameWithIndex::Looper>();
			} else if (txt.starts_with("13:"sv)) {
				looper->DelaySwitchTo<MovePath::Looper>();
			} else if (txt.starts_with("14:"sv)) {
				looper->DelaySwitchTo<Scissor::Scene>();
			} else if (txt.starts_with("15:"sv)) {
				looper->DelaySwitchTo<RenderTextureTest::Scene>();
			} else if (txt.starts_with("16:"sv)) {
				looper->DelaySwitchTo<AudioTest::Scene>();
			} else if (txt.starts_with("17:"sv)) {
				looper->DelaySwitchTo<ImGuiTest::Scene>();
			} else if (txt.starts_with("18:"sv)) {
				looper->DelaySwitchTo<PolygonSpriteTest::Scene>();
			} else if (txt.starts_with("19:"sv)) {
				looper->DelaySwitchTo<SpaceShooter::Scene>();
			} else if (txt.starts_with("20:"sv)) {
				looper->DelaySwitchTo<ParticleTest::Scene>();
			} else if (txt.starts_with("21:"sv)) {
				looper->DelaySwitchTo<XxmvTest::Scene>();
			} else {
				throw std::logic_error("unhandled menu");
			}
				//looper->DelaySwitchTo<::Scene>();	// todo: circle editor ?
				//looper->DelaySwitchTo<::Scene>();	// todo: anim frame editor ?
		}
	}

	void Menu::Init(GameLooper* looper, xx::XY const& pos, std::string_view const& txt_, float const& fontSize) {
		this->looper = looper;
		txt = txt_;
		content.SetText(looper->fontBase, txt, fontSize, xx::engine.w / 3 - 50).SetPosition(pos);
		auto hw = content.size.x / 2;
		auto hh = content.size.y / 2;
		leftBottom = { pos.x - hw, pos.y - hh };
		rightTop = { pos.x + hw, pos.y + hh };
	}

	bool Menu::Inside(xx::XY const& p) {
		return p.x >= leftBottom.x && p.x <= rightTop.x
			&& p.y >= leftBottom.y && p.y <= rightTop.y;
	}

}
