#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

#define PERFORMANCE_TEST

struct PositionRadius_ {
	float x, y, r;
};

struct Monster : PositionRadius_ {
	int hp;
};

struct Cannon : PositionRadius_ {
	int power;
	int nextShootFrameNumber;
};

struct Bullet : PositionRadius_ {
	float xInc, yInc;
	int damage;			// copy from cannon's power
	int timeoutFrameNumber;
};

struct Scene {
	std::vector<Monster> monsters;
	std::vector<Cannon> cannons;
	std::vector<Bullet> bullets;
	int frameNumber = 0;

	const int totalNumMonsters = 100;

	void Init() {
		for (int i = 0; i < totalNumMonsters; ++i) {
			auto& o = monsters.emplace_back();
			o.x = 0;
			o.y = 0;
			o.r = 10;
			o.hp = 300;
		}
		for (int i = 0; i < 20; ++i) {
			auto& c = cannons.emplace_back();
			c.x = i * 10.f;
			c.y = 20;
			c.r = 100;
			c.power = 50;
			c.nextShootFrameNumber = 0;
		}
	}

	template<typename A, typename B>
	bool HitCheck(A const& a, B const& b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) < (a.r + b.r) * (a.r + b.r);
	}

	template<typename Point1, typename Point2>
	void GetXYInc(Point1 const& from, Point2 const& to, float const& speed, float& xInc, float& yInc) noexcept {
		if (from.x == to.x && from.y == to.y) {
			xInc = yInc = 0;
			return;
		}
		auto a = atan2f(to.y - from.y, to.x - from.x);
		xInc = speed * cosf(a);
		yInc = speed * sinf(a);
	}

	void Update() {
		++frameNumber;

		// 子弹移动 并判断是否命中某怪. 命中则减血删子弹. 血 <=0 则删怪
		for (int i = (int)bullets.size() - 1; i >= 0; --i) {
			auto& b = bullets[i];
			if (b.timeoutFrameNumber <= frameNumber) {
				b = bullets.back();
				bullets.pop_back();
				continue;
			}
			b.x += b.xInc;
			b.y += b.yInc;
			for (int j = (int)monsters.size() - 1; j >= 0; --j) {
				auto& m = monsters[j];
				if (HitCheck(b, m)) {
					m.hp -= b.damage;
					b = bullets.back();
					bullets.pop_back();
					if (m.hp <= 0) {
						m = monsters.back();
						monsters.pop_back();
					}
					break;
				}
			}
		}

		// 怪物移动
		for (auto& m : monsters) {
			m.x += 1;
			if (m.x >= 200) m.x = 0;
		}

#ifdef PERFORMANCE_TEST
		// 判断如果怪数量少了，就补
		for (int i = totalNumMonsters - (int)monsters.size(); i > 0; --i) {
			auto& o = monsters.emplace_back();
			o.x = 0;
			o.y = 0;
			o.r = 10;
			o.hp = 300;
		}
#endif

		// 判断是否有怪在射程内 且自己 cd 时间到了, 就发射 bullet
		for (auto& c : cannons) {
			if (c.nextShootFrameNumber > frameNumber) continue;
			for (auto& m : monsters) {
				if (HitCheck(c, m)) {
					auto& b = bullets.emplace_back();
					b.x = c.x;
					b.y = c.y;
					b.r = 5;
					GetXYInc(b, m, 5, b.xInc, b.yInc);	// todo: 取提前量??
					b.damage = c.power;
					b.timeoutFrameNumber = frameNumber + 20;		// 多少帧内没命中怪的话子弹就自杀

					c.nextShootFrameNumber = frameNumber + 10;		// 设置射击 cd
					break;
				}
			}
		}

#ifndef PERFORMANCE_TEST
		std::cout << "frameNumber = " << frameNumber
			<< ", monsters.size() = " << monsters.size()
			//<< ", cannons.size() = " << cannons.size()
			<< ", bullets.size() = " << bullets.size() << std::endl;
#endif
	}
};

int main() {
	Scene scene;
	scene.Init();

	int num = 1000000;
	auto tp = std::chrono::system_clock::now();
	for (int i = 0; i < num; ++i) {
		scene.Update();
#ifndef PERFORMANCE_TEST
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
