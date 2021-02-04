#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <array>
#include "xx_ptr.h"
#include "Space2dPointIndex.h"

// tower defence simulate

#define MOVE_ONLY_STRUCT_CODE(T) \
T() = default; \
T(T const&) = delete; \
T& operator=(T const&) = delete; \
T(T&&) = default; \
T& operator=(T&&) = default;

struct Scene;
struct Bullet;
struct Monster {
	MOVE_ONLY_STRUCT_CODE(Monster);
	float x, y, r;				// 坐标，半径
	float x_, y_;				// 上一帧的数据
	int hp;						// 血量
	std::array<char, 64> dummy; // 模拟各种上下文废料

	void OnCreate(Scene& s, float const& x, float const& y);
	void OnHitEffect(Scene& s, Bullet& b);
	void OnMove(Scene& s);
	void OnDestroy(Scene& s);
};

struct Cannon {
	MOVE_ONLY_STRUCT_CODE(Cannon);
	float x, y, r;				// 坐标，警戒半径
	float pipeLen;				// 炮管长度( 子弹从炮口开始飞行 )
	float bulletRadius;			// 射出的子弹半径
	float bulletSpeed;			// 射出的子弹飞行速度( 每帧距离 )
	int bulletDamage;			// 射出的子弹威力值
	int bulletLife;				// 射出的子弹能存活多少帧
	int shootDelay;				// 发射延迟
	int nextShootFrameNumber;	// 下次可发射的帧编号
	xx::Weak<Monster> target;	// 当前目标( 优先锁定第一个出现在警戒半径里的. 后锁定正在远离自己最远的 )
	std::array<char, 64> dummy; // 模拟各种上下文废料

	void OnCreate(Scene& s, float const& x, float const& y);
	void OnUpdate(Scene& s);
	void OnDestroy(Scene& s);
};

struct Bullet {
	MOVE_ONLY_STRUCT_CODE(Bullet);
	float x, y, r;				// 坐标，半径
	float xInc, yInc;			// 每帧 x,y 移动增量
	int damage;
	int timeoutFrameNumber;		// 子弹失效帧编号
	std::array<char, 64> dummy; // 模拟各种上下文废料

	void OnCreate(Scene& s, Cannon& c, Monster& m, int const& frameNumber);
	void OnMove(Scene& s);
	void OnDestroy(Scene& s);
};


template<typename A, typename B>
float CalcXXYY(A const& a, B const& b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}
template<typename A, typename B>
float CalcRR(A const& a, B const& b) {
	return (a.r + b.r) * (a.r + b.r);
}
template<typename A, typename B>
bool HitCheck(A const& a, B const& b) {
	return CalcXXYY(a, b) < CalcRR(a, b);
}

template<typename A, typename B>
void GetXYInc(A const& from, B const& to, float const& speed, float& xInc, float& yInc) noexcept {
	if (from.x == to.x && from.y == to.y) {
		xInc = yInc = 0;
		return;
	}
	auto a = atan2f(to.y - from.y, to.x - from.x);
	xInc = speed * cosf(a);
	yInc = speed * sinf(a);
}

struct Scene {
	const int totalNumMonsters = 100;
	std::vector<xx::Shared<Monster>> monsters;
	std::vector<Cannon> cannons;
	std::vector<Bullet> bullets;
	int frameNumber = 0;

	void Init() {
		for (int i = 0; i < totalNumMonsters; ++i) {
			monsters.emplace_back().Emplace()->OnCreate(*this, i * 1.f, 0.f);
		}
		for (int i = 0; i < 20; ++i) {
			cannons.emplace_back().OnCreate(*this, i * 10.f, 20.f);
		}
	}

	// 找出所有能命中的怪当中 离得最近的( 子弹碰撞判断逻辑 )
	int FindNearestMonster(Bullet& o) {
		int idx = -1;
		float minXXYY = 9999999;
		for (int siz = (int)monsters.size(), j = 0; j < siz; ++j) {
			auto& m = monsters[j];
			auto xxyy = CalcXXYY(o, *m);
			auto rr = CalcRR(o, *m);
			if (xxyy < rr && minXXYY >xxyy) {
				minXXYY = xxyy;
				idx = j;
			}
		}
		return idx;
	}

	// 找出所有能命中的怪当中 最应该攻击的( 优先打正在远离的最远的. 否则打最近的 )( 炮台逻辑 )
	int FindTargetMonster(Cannon& o) {
		int minIdx = -1;
		float minXXYY = 9999999;
		int maxIdx = -1;
		float maxXXYY = 0;
		for (int siz = (int)monsters.size(), j = 0; j < siz; ++j) {
			auto& m = monsters[j];
			auto xxyy = CalcXXYY(o, *m);
			auto rr = CalcRR(o, *m);
			if (xxyy < rr) {
				auto xxyy_ = (o.x - m->x_) * (o.x - m->x_) + (o.y - m->y_) * (o.y - m->y_);
				// 如果怪 上一个坐标 离自己更近: 正在远离
				if (xxyy > xxyy_) {
					if (maxXXYY < xxyy) {
						maxXXYY = xxyy;
						maxIdx = j;
					}
				}
				else {
					if (minXXYY > xxyy) {
						minXXYY = xxyy;
						minIdx = j;
					}
				}
			}
		}
		if (maxIdx != -1) return maxIdx;
		return minIdx;
	}


	void Update() {
		++frameNumber;

		// 子弹移动 并判断是否命中某怪. 命中则减血删子弹. 血 <=0 则删怪
		for (int i = (int)bullets.size() - 1; i >= 0; --i) {
			auto& b = bullets[i];
			if (b.timeoutFrameNumber <= frameNumber) {
				b.OnDestroy(*this);
				b = std::move(bullets.back());
				bullets.pop_back();
				continue;
			}
			b.x += b.xInc;
			b.y += b.yInc;

			// 找出所有能命中的怪当中 离得最近的
			int idx = FindNearestMonster(b);
			if (idx != -1) {
				auto& m = monsters[idx];
				m->OnHitEffect(*this, b);
				m->hp -= b.damage;
				b.OnDestroy(*this);
				b = std::move(bullets.back());
				bullets.pop_back();
				if (m->hp <= 0) {
					m->OnDestroy(*this);
					m = std::move(monsters.back());
					monsters.pop_back();
				}
			}
			else {
				b.OnMove(*this);
			}
		}

		// 怪物移动
		for (auto& m : monsters) {
			m->x += 1;
			if (m->x >= 200) m->x = 0;
			m->OnMove(*this);
		}

#ifdef NDEBUG
		// 判断如果怪数量少了，就补
		for (int i = totalNumMonsters - (int)monsters.size(); i > 0; --i) {
			monsters.emplace_back().Emplace()->OnCreate(*this, 0.f, 0.f);
		}
#endif

		// 判断是否有怪在射程内 且自己 cd 时间到了, 就发射 bullet
		for (auto& c : cannons) {
			c.OnUpdate(*this);
			if (c.nextShootFrameNumber > frameNumber) continue;
			// 判断 target 是否已出范围. 有效 & 未出范围 继续攻击
			if (auto m = c.target.Lock()) {
				if (HitCheck(c, *m)) {
					bullets.emplace_back().OnCreate(*this, c, *m, frameNumber);	// 发射子弹
					c.nextShootFrameNumber = frameNumber + c.shootDelay;	// 设置射击 cd
				}
			}
			// 如果已出 则重新选.  优先选正在远离自己的最远的
			else {
				int idx = FindTargetMonster(c);
				if (idx != -1) {
					auto& m = monsters[idx];
					c.target = m;
					bullets.emplace_back().OnCreate(*this, c, *m, frameNumber);	// 发射子弹
					c.nextShootFrameNumber = frameNumber + c.shootDelay;	// 设置射击 cd
					break;
				}
			}
		}

		// 怪物移动
		for (auto& m : monsters) {
			m->x_ = m->x;
			m->y_ = m->y;
		}

#ifndef NDEBUG
		std::cout << "frameNumber = " << frameNumber
			<< ", monsters.size() = " << monsters.size()
			//<< ", cannons.size() = " << cannons.size()
			<< ", bullets.size() = " << bullets.size() << std::endl;
#endif
	}
	};

inline void Monster::OnCreate(Scene& s, float const& x, float const& y) {
	this->x = this->x_ = x;
	this->y = this->y_ = y;
	this->r = 10.f;
	this->hp = 300;
	// draw?
}
inline void Monster::OnHitEffect(Scene& s, Bullet& b) {
	// play -hp effect?
}
inline void Monster::OnMove(Scene& s) {
	// update draw?
}
inline void Monster::OnDestroy(Scene& s) {
	// clear draw?
}


inline void Cannon::OnCreate(Scene& s, float const& x, float const& y) {
	this->x = x;
	this->y = y;
	this->r = 100.f;
	this->pipeLen = 5.f;
	this->bulletRadius = 5.f;
	this->bulletSpeed = 5.f;
	this->bulletDamage = 50;
	this->bulletLife = 30;
	this->shootDelay = 6;
	this->nextShootFrameNumber = 0;
}
inline void Cannon::OnUpdate(Scene& s) {
	// update draw?
}
inline void Cannon::OnDestroy(Scene& s) {
	// clear draw?
}


inline void Bullet::OnCreate(Scene& s, Cannon& c, Monster& m, int const& frameNumber) {
	GetXYInc(c, m, c.bulletSpeed, this->xInc, this->yInc);		// 得到 x,y 每帧增量
	this->x = c.x + this->xInc / c.bulletSpeed * c.pipeLen;		// 将子弹坐标初始化到炮口位置
	this->y = c.y + this->yInc / c.bulletSpeed * c.pipeLen;
	this->r = c.bulletRadius;									// 设置子弹半径
	this->damage = c.bulletDamage;								// 设置子弹威力
	this->timeoutFrameNumber = frameNumber + c.bulletLife;		// 多少帧内没命中怪的话子弹就自杀
	// draw?
}
inline void Bullet::OnMove(Scene& s) {
	// update draw?
}
inline void Bullet::OnDestroy(Scene& s) {
	// clear draw?
}

int main() {
	Scene scene;
	scene.Init();

	int num = 1000000;
	auto tp = std::chrono::system_clock::now();
	for (int i = 0; i < num; ++i) {
		scene.Update();
#ifndef NDEBUG
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
#endif
	}
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "update " << num << " times. ms = " << ms << std::endl;

	return 0;
	}












//#include "readerwriterqueue.h"
//#include <thread>
//#include <iostream>
//#include <chrono>
//#include <string>
//#include <string_view>
////#include <vector>
//
//int main() {
//	moodycamel::ReaderWriterQueue<std::string> q(10000000);
//	std::thread t1([&] {
//		auto tp = std::chrono::system_clock::now();
//		for (int i = 1; i <= 10000000; i++) {
//			if (!q.enqueue(std::to_string(i))) throw - 1;
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "write finished. ms = " << ms << std::endl;
//		});
//	std::thread t2([&] {
//		auto tp = std::chrono::system_clock::now();
//		std::string v;
//		while (true) {
//			if (q.try_dequeue(v)) {
//				if (v == "10000000") break;
//			}
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << " ms = " << ms << std::endl;
//		});
//	t1.join();
//	t2.join();
//}




////#define XX_LOG_USE_XX_MUTEX
//#include "xx_logger.h"
//
//int main() {
//	__xxLogger.cfg.outputConsole = false;
//	for (size_t i = 0; i < 100000; i++) {
//		LOG_INFO(i);
//	}
//	auto tp = std::chrono::system_clock::now();
//	std::vector<std::thread> threads;
//	for (int i = 0; i < 40; ++i) {
//		threads.emplace_back([] {
//			for (size_t i = 0; i < 100000; i++) {
//				LOG_INFO(i);
//			}
//			}
//		);
//	}
//	for (auto& t : threads) t.join();
//
//	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//	std::cout << "ms = " << ms << std::endl;
//	return 0;
//}




//#include <thread>
//#include <iostream>
//#include <chrono>
//#include <mutex>
//#include <vector>
//#include <string>
//#include <string_view>
//#ifdef _WIN32
//#define NOMINMAX
//#define NODRAWTEXT
//#define NOGDI
//#define NOBITMAP
//#define NOMCX
//#define NOSERVICE
//#define NOHELP
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#else
//#include <unistd.h>
//void Sleep(int const& ms) {
//	usleep(ms * 1000);
//}
//#endif
//
//#ifndef NDEBUG
//#define XX_NOINLINE
//#define XX_FORCEINLINE
//#else
//#ifdef _MSC_VER
//#define XX_NOINLINE __declspec(noinline)
//#define XX_FORCEINLINE __forceinline
//#else
//#define XX_NOINLINE __attribute__((noinline))
//#define XX_FORCEINLINE __attribute__((always_inline))
//#endif
//#endif
//#define XX_INLINE inline
//
//class Mutex {
//	std::atomic_flag flag = ATOMIC_FLAG_INIT;
//public:
//	XX_FORCEINLINE void lock() {
//		while (flag.test_and_set(std::memory_order_acquire)) Sleep(1);    // std::this_thread::yield()  // std::this_thread::sleep_for(std::chrono::microseconds(1));
//	}
//	XX_FORCEINLINE void unlock() {
//		flag.clear(std::memory_order_release);
//	}
//};
//
//
////uint64_t c = 0;
////XX_NOINLINE void DoSth() {
////    ++c;
////}
//std::vector<std::string> strs;
//XX_NOINLINE void DoSth(std::string_view const& s) {
//	strs.emplace_back(s);
//}
//
//
//std::mutex mtx2;
//void CallFunc1() {
//	for (size_t i = 0; i < 1000000; i++) {
//		std::lock_guard<std::mutex> lg(mtx2);
//		for (int j = 0; j < 10; ++j) {
//			DoSth(std::to_string(i * j));
//		}
//	}
//}
//
//Mutex mtx;
//void CallFunc2() {
//	for (size_t i = 0; i < 1000000; i++) {
//		mtx.lock();
//		for (int j = 0; j < 10; ++j) {
//			DoSth(std::to_string(i * j));
//		}
//		mtx.unlock();
//	}
//}
//
//int main() {
//	{
//		strs.clear();
//		auto tp = std::chrono::system_clock::now();
//		{
//			std::vector<std::thread> threads;
//			for (int i = 0; i < 1; ++i) {
//				threads.emplace_back(&CallFunc2);
//			}
//			for (auto& t : threads) t.join();
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "strs.size() = " << strs.size() << " ms = " << ms << std::endl;
//	}
//	{
//		strs.clear();
//		auto tp = std::chrono::system_clock::now();
//		{
//			std::vector<std::thread> threads;
//			for (int i = 0; i < 1; ++i) {
//				threads.emplace_back(&CallFunc1);
//			}
//			for (auto& t : threads) t.join();
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "strs.size() = " << strs.size() << " ms = " << ms << std::endl;
//	}
//	{
//		strs.clear();
//		auto tp = std::chrono::system_clock::now();
//		{
//			std::vector<std::thread> threads;
//			for (int i = 0; i < 1; ++i) {
//				threads.emplace_back(&CallFunc2);
//			}
//			for (auto& t : threads) t.join();
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "strs.size() = " << strs.size() << " ms = " << ms << std::endl;
//	}
//	return 0;
//}




//#include <thread>
//#include <iostream>
//#include <chrono>
//#include <mutex>
//#include <vector>
//#include <string_view>
//#include <atomic>
//
//#define NOMINMAX
//#define NODRAWTEXT
//#define NOGDI
//#define NOBITMAP
//#define NOMCX
//#define NOSERVICE
//#define NOHELP
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//
//class Mutex {
//    std::atomic<bool> flag{};
//public:
//    void lock() {
//        while (flag.exchange(true, std::memory_order_relaxed)) Sleep(0);
//        std::atomic_thread_fence(std::memory_order_acquire);
//    }
//
//    void unlock() {
//        std::atomic_thread_fence(std::memory_order_release);
//        flag.store(false, std::memory_order_relaxed);
//    }
//};
//
//Mutex mtx;
//std::mutex mtx2;
//uint64_t c = 0;
//
//__declspec(noinline) void DoSomeThing() {
//    ++c;
//}
//
//__declspec(noinline) void Func1() {
//    std::lock_guard<std::mutex> lg(mtx2);
//    DoSomeThing();
//}
//
//__declspec(noinline) void Func2() {
//    mtx.lock();
//    DoSomeThing();
//    mtx.unlock();
//}
//
//__declspec(noinline) void CallFunc1() {
//    for (size_t i = 0; i < 1000000; i++) {
//        Func1();
//    }
//}
//__declspec(noinline) void CallFunc2() {
//    for (size_t i = 0; i < 1000000; i++) {
//        Func2();
//    }
//}
//
//int main() {
//    {
//        auto tp = std::chrono::system_clock::now();
//        {
//            std::vector<std::thread> threads;
//            for (int i = 0; i < 4; ++i) {
//                threads.emplace_back(&CallFunc1);
//            }
//            for (auto& t : threads) t.join();
//        }
//        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//        std::cout << "c = " << c << " ms = " << ms << std::endl;
//    }
//    {
//        c = 0;
//        auto tp = std::chrono::system_clock::now();
//        {
//            std::vector<std::thread> threads;
//            for (int i = 0; i < 4; ++i) {
//                threads.emplace_back(&CallFunc2);
//            }
//            for (auto& t : threads) t.join();
//        }
//        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//        std::cout << "c = " << c << " ms = " << ms << std::endl;
//    }
//    return 0;
//}




//#include <thread>
//#include <iostream>
//#include <chrono>
//#include <mutex>
//#include <vector>
//#include <string_view>
//#include <atomic>
//
//// Use the C++ standard templated min/max
//#define NOMINMAX
//
//// DirectX apps don't need GDI
//#define NODRAWTEXT
//#define NOGDI
//#define NOBITMAP
//
//// Include <mcx.h> if you need this
//#define NOMCX
//
//// Include <winsvc.h> if you need this
//#define NOSERVICE
//
//// WinHelp is deprecated
//#define NOHELP
//
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//
//
//class Mutex {
//	std::atomic<bool> flag{ false };
//public:
//	void lock()
//	{
//		while (flag.exchange(true, std::memory_order_relaxed)) Sleep(0);
//		std::atomic_thread_fence(std::memory_order_acquire);
//	}
//
//	void unlock()
//	{
//		std::atomic_thread_fence(std::memory_order_release);
//		flag.store(false, std::memory_order_relaxed);
//	}
//};
//
//struct Logger {
//	std::vector<std::string> logs;
//	inline void Log(std::string_view const& v) {
//		logs.emplace_back(v);
//	}
//};
////std::mutex mtx;
//Mutex mtx;
//std::vector<Logger*> loggers;
//thread_local std::unique_ptr<Logger> logger;
//
//__declspec(noinline) void Log(std::string_view const& v) {
//	if (!logger) {
//		//std::lock_guard<std::mutex> lg(mtx);
//		mtx.lock();
//		logger.reset(loggers.emplace_back(new Logger()));
//		logger->logs.reserve(10000000);
//		mtx.unlock();
//	}
//	logger->Log(v);
//}
//
//struct Logger2 {
//	std::vector<std::string> logs;
//	__declspec(noinline) void Log(std::string_view const& v) {
//		//std::lock_guard<std::mutex> lg(mtx);
//		mtx.lock();
//		logs.emplace_back(v);
//		mtx.unlock();
//	}
//};
//Logger2 logger2;
//
//int main() {
//	{
//		auto tp = std::chrono::system_clock::now();
//		for (size_t i = 0; i < 10000000; i++) {
//			Log("asdf");
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "Logger logs size = " << loggers[0]->logs.size() << " ms = " << ms << std::endl;
//	}
//	{
//		auto tp = std::chrono::system_clock::now();
//		for (size_t i = 0; i < 10000000; i++) {
//			logger2.Log("asdf");
//		}
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
//		std::cout << "Logger2 logs size = " << logger2.logs.size() << " ms = " << ms << std::endl;
//	}
//	return 0;
//}
