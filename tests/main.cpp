#include "main.h"

#include <xx_listdoublelink.h>
int main() {
	int counter, n = 1, m = 5000000;
	auto secs = xx::NowEpochSeconds();

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		xx::ListLink<int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			new (&ll.Add()) int(j);
		}

		ll.Foreach([](auto& o)->bool {
			return o == 2 || o == 4;
			});
		new (&ll.Add()) int(2);
		new (&ll.Add()) int(4);

		ll.Foreach([&](auto& o) {
			counter += o;
			});
	}

	xx::CoutN("ListLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		xx::ListDoubleLink<int, int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			ll.Emplace(j);
		}

		ll.Foreach([](auto& o)->bool {
			return o == 2 || o == 4;
			});
		ll.Emplace(2);
		ll.Emplace(4);

		ll.Foreach([&](auto& o) {
			counter += o;
			});
	}

	xx::CoutN("ListDoubleLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::map<int, int> ll;
		int autoInc{};
		for (size_t j = 1; j <= m; j++) {
			ll[++autoInc] = j;
		}

		for (auto it = ll.begin(); it != ll.end();) {
			if (it->second == 2 || it->second == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll[++autoInc] = 2;
		ll[++autoInc] = 4;

		for (auto&& kv : ll) {
			counter += kv.second;
		}
	}

	xx::CoutN("map counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));


	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::list<int> ll;
		for (size_t j = 1; j <= m; j++) {
			ll.push_back(j);
		}

		for (auto it = ll.begin(); it != ll.end();) {
			if (*it == 2 || *it == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll.push_back(2);
		ll.push_back(4);

		for (auto&& v : ll) {
			counter += v;
		}
	}

	xx::CoutN("list counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));


	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::pmr::list<int> ll;
		for (size_t j = 1; j <= m; j++) {
			ll.push_back(j);
		}

		for (auto it = ll.begin(); it != ll.end();) {
			if (*it == 2 || *it == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll.push_back(2);
		ll.push_back(4);

		for (auto&& v : ll) {
			counter += v;
		}
	}

	xx::CoutN("pmr list counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	return 0;

	{
		struct Scene;
		struct Monster;
		using Monsters = std::list<std::shared_ptr<Monster>>;

		struct Monster {
			Scene* scene;
			Monsters::iterator iter;
			std::weak_ptr<Monster> target;
			Monster(Scene* const& scene, Monsters::iterator const& iter) : scene(scene), iter(iter) {}
			bool Update() { return true; }
			void Draw() {}
		};

		struct Scene {
			Monsters monsters;
			void Run() {
				for (size_t i = 0; i < 100; i++) {
					auto& m = monsters.emplace_back(std::make_shared<Monster>(this, Monsters::iterator{} ));
					m->iter = monsters.rbegin().base();
				}
				for (auto&& iter = monsters.rbegin(); iter != monsters.rend();) {
					if ( (*iter)->Update() ) {
						iter = Monsters::reverse_iterator(monsters.erase(iter.base()));
					} else {
						++iter;
					}
				}
				for (auto&& m : monsters) {
					m->Draw();
				}
			}
		};

		Scene scene;
		scene.Run();
	}


	{
		struct Scene;
		struct Monster;
		using Monsters = xx::ListDoubleLink<xx::Shared<Monster>>;

		struct Monster {
			Scene* scene;
			Monsters::IndexAndVersion iter;
			xx::Weak<Monster> target;
			Monster(Scene* const& scene, Monsters::IndexAndVersion const& iter) : scene(scene), iter(iter) {}
			bool Update() { return true; }
			void Draw() {}
		};

		struct Scene {
			Monsters monsters;
			void Run() {
				for (size_t i = 0; i < 100; i++) {
					monsters.Add()(xx::Make<Monster>(this, monsters.GetTailIndexAndVersion()));
				}
				monsters.ForeachReverse([](auto& m)->bool {
					return m->Update();
				});
				monsters.Foreach([](auto& m) {
					m->Draw();
				});
			}
		};

		Scene scene;
		scene.Run();
	}

	return 0;
}
