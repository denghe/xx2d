#include "main.h"
#include "s0_main_menu.h"
#include "imgui.h"

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

	scene = xx::Make<MainMenu::Scene>();
	scene->Init(this);

	xx::engine.imguiInit = [] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();

		//ImFontConfig cfg;
		//cfg.SizePixels = 26.f;
		//auto&& imfnt = io.Fonts->AddFontDefault(&cfg);
		auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());

		io.Fonts->Build();
		io.FontDefault = imfnt;
	};
}

int GameLooper::Update() {
	if (xx::engine.Pressed(xx::KbdKeys::Escape)) {
		if (!dynamic_cast<MainMenu::Scene*>(scene.pointer)) {
			DelaySwitchTo<MainMenu::Scene>();
		}
	}

	int r = scene->Update();

	fpsViewer.Update();
	return r;
}









namespace xx {

	template<typename T, typename SizeType = ptrdiff_t>
	struct ListLink {

		struct Node {
			SizeType next;
			T value;
		};

		Node* buf{};
		SizeType cap{}, len{};
		SizeType head{ -1 }, tail{ -1 }, freeList{ -1 }, freeCount{};

		ListLink() = default;
		ListLink(ListLink const&) = delete;
		ListLink& operator=(ListLink const&) = delete;
		~ListLink() {
			if (!cap) return;
			if constexpr (IsPod_v<T>) {
				while (head >= 0) {
					buf[head].value.~T();
					head = buf[head].next;
				}
			}
			::free(buf);
			buf = {};
			cap = 0;
			Clear();
		}

		void Reserve(SizeType const& cap_) noexcept {
			assert(cap_ > 0);
			if (cap_ <= cap) return;
			if (!cap) {
				buf = (Node*)::malloc(cap_ * sizeof(Node));
				cap = cap_;
				return;
			}
			do {
				cap *= 2;
			} while (cap < cap_);
			auto newBuf = (Node*)::malloc(cap * sizeof(Node));

			if constexpr (IsPod_v<T>) {
				::memcpy((void*)newBuf, (void*)buf, len * sizeof(Node));
			} else {
				for (SizeType i = 0; i < len; ++i) {
					new (&newBuf[i].value) T((T&&)buf[i].value);
					buf[i].value.~T();
				}
			}
			::free(buf);
			buf = newBuf;
		}

		// return value: new ( ll.Add() ) T( ... );
		T* Add() {
			SizeType idx;
			if (freeCount > 0) {
				idx = freeList;
				freeList = buf[idx].next;
				freeCount--;
			} else {
				if (len == cap) {
					Reserve(cap ? cap * 2 : 64);
				}
				idx = len;
				len++;
			}

			buf[idx].next = -1;

			if (tail >= 0) {
				buf[tail].next = idx;
				tail = idx;
			} else {
				assert(head == -1);
				head = tail = idx;
			}

			return &buf[idx].value;
		}

		// return next index
		SizeType Remove(SizeType const& idx, SizeType const& prevIdx = -1) {
			assert(idx >= 0);
			assert(idx < len);

			if (idx == head) {
				head = buf[idx].next;
			}
			if (idx == tail) {
				tail = prevIdx;
			}
			auto r = buf[idx].next;
			if (prevIdx >= 0) {
				buf[prevIdx].next = r;
			}
			buf[idx].value.~T();
			buf[idx].next = freeList;
			freeList = idx;
			++freeCount;
			return r;
		}

		SizeType Next(SizeType const& idx) const {
			return buf[idx].next;
		}

		void Clear() {
			head = tail = freeList = -1;
			freeCount = 0;
			len = 0;
		}

		T const& operator[](SizeType const& idx) const noexcept {
			assert(idx < len);
			return buf[idx].value;
		}
		T& operator[](SizeType const& idx) noexcept {
			assert(idx < len);
			return buf[idx].value;
		}
	};
}


int main() {

	xx::ListLink<int, int> ll;
	*ll.Add() = 1;
	*ll.Add() = 2;
	*ll.Add() = 3;

	int prev = -1, next{};
	for (auto idx = ll.head; idx != -1;) {

		if (ll[idx] == 2) {
			next = ll.Remove(idx, prev);
		} else {
			next = ll.Next(idx);
		}
		prev = idx;
		idx = next;
	}

	for (auto idx = ll.head; idx != -1; idx = ll.Next(idx)) {
		std::cout << ll[idx] << std::endl;
	}
	return 0;

	//return GameLooper{}.Run("xx2d's examples");
}
