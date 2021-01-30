#include "readerwriterqueue.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <string_view>
//#include <vector>

int main() {
	moodycamel::ReaderWriterQueue<std::string> q(10000000);
	std::thread t1([&] {
		auto tp = std::chrono::system_clock::now();
		for (int i = 1; i <= 10000000; i++) {
			if (!q.enqueue(std::to_string(i))) throw - 1;
		}
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
		std::cout << "write finished. ms = " << ms << std::endl;
		});
	std::thread t2([&] {
		auto tp = std::chrono::system_clock::now();
		std::string v;
		while (true) {
			if (q.try_dequeue(v)) {
				if (v == "10000000") break;
			}
		}
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
		std::cout << " ms = " << ms << std::endl;
		});
	t1.join();
	t2.join();
}




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
