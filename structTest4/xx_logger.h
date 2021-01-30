#include "xx_fixeddata_w.h"
#include "ajson.hpp"
#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <sstream>

// 如果要使用自己定义的日志实例和宏，就在包含之前 #define XX_USE_CUSTOM_LOG_DEFINES

//#define XX_LOG_USE_XX_MUTEX

namespace xx {

    // 针对 __FILE__ 编译期 定位到纯文件名部分并返回指针
    template<size_t len>
    inline constexpr char const *CutPath(char const(&in)[len]) {
        auto &&i = len - 1;
        for (; i >= 0; --i) {
            if (in[i] == '\\' || in[i] == '/') return in + i + 1;
        }
        return in + i;
    }

    // 获取当前执行文件名字
    inline std::string GetExecuteName() {
#if defined(PLATFORM_POSIX) || defined(__linux__)
        std::string s;
        std::ifstream("/proc/self/comm") >> s;
        return s;
#elif defined(_WIN32)
        //char buf[MAX_PATH];
        //GetModuleFileNameA(nullptr, buf, MAX_PATH);
        //return buf;
        return "unnamed.exe";   // 为避免在此处引入 windows.h .可以通过配置解决
#else
        static_assert(false, "unrecognized platform");
#endif
    }

    // 将时间转为 2020-08-01 13:56:48.123456 这样的长相
    inline std::string
    ToString(std::chrono::system_clock::time_point const &tp, char const *const &format = "%F %T") noexcept {
        auto &&t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::stringstream ss;
        ss << std::put_time(&tm, format);
//        auto &&e = tp.time_since_epoch();
//        ss << "." << std::chrono::duration_cast<std::chrono::microseconds>(e).count() -
//                     std::chrono::duration_cast<std::chrono::seconds>(e).count() * 1000000LL;
        return ss.str();
    }

    // 日志级别
    enum class LogLevels : int {
        TRACE, DEBUG, INFO, WARN, ERR
    };
    inline char const *logLevelNames[] = {
            "TRACE", "DEBUG", "INFO", "WARN", "ERR"
    };

    // 带颜色的 日志级别串（ERROR 那个是红色，别的乱来的）
    inline char const *logLevelColorNames[] = {
            "\033[35mTRACE\033[37m", "\033[32mDEBUG\033[37m", "\033[33mINFO\033[37m", "\033[34mWARN\033[37m",
            "\033[31mERROR\033[37m"
    };

    /***********************************************************************************************/
    // 日志配置
    /***********************************************************************************************/

    /*
json 样板:
{
    "logLevel" : 1
    , "logFileName" : "log/server.log"
    , "logFileMaxBytes" : 5242880
    , "logFileCount" : 30
    , "outputConsole" : true
}
     */
    struct LoggerConfig {
        // 小于等于这个级别的才记录日志
        int logLevel = (int) LogLevels::TRACE;
        // 日志文件 路径 & 文件名前缀( 后面可能还有日期 / 分段标志 )
        std::string logFileName = GetExecuteName() + ".log";
        // 单个日志文件体积上限( 字节 )
        int64_t logFileMaxBytes = 1024 * 1024 * 5;
        // 日志文件最多个数( 滚动使用，超过个数将删除最早的 )
        int logFileCount = 30;
        // 写文件的同时, 是否同时输出到控制台
        bool outputConsole = true;

        LoggerConfig() = default;

        LoggerConfig(LoggerConfig const &) = default;

        LoggerConfig &operator=(LoggerConfig const &) = default;

        ~LoggerConfig() = default;
    };

}
AJSON(xx::LoggerConfig, logLevel, logFileName, logFileMaxBytes, logFileCount, outputConsole);
namespace xx {

    // 适配 std::cout
    inline std::ostream &operator<<(std::ostream &o, LoggerConfig const &c) {
        ajson::save_to(o, c);
        return o;
    }

    /***********************************************************************************************/
    // 日志类主体
    /***********************************************************************************************/

    struct Logger {
        // 每条日志保存容器数据类型（定长 256 字节 data )
        typedef FixedData<256> Item;

        // 后台线程扫描写入队列的休息时长（毫秒）
        inline static const int64_t loopSleepMS = 1;

        // 后台线程重新加载 cfg 文件的间隔时长（秒）
        inline static const int64_t reloadConfigIntervalSeconds = 60;

        // 当前配置（如果没有配置文件则可直接改。否则会被重新加载覆盖掉）
        LoggerConfig cfg;
    protected:
        // 前后台队列
        std::vector<Item> items1;
        std::vector<Item> items2;

        // 后台线程 & Lock 系列
        std::thread thread;
#ifdef XX_LOG_USE_XX_MUTEX
        Mutex mtx;
#else
        std::mutex mtx;
#endif

        // 是否正在析构
        volatile int disposing = 0;

        // 后台是否没有正在写
        bool writing = false;

        // 当前默认配置文件名
        std::string cfgName = "xxlog.config";

        // 当前日志文件名( 日志主体名 + 时间 )
        std::string currLogFileName;

        // 指向日志文件的句柄
        std::ofstream ofs;

        // 已经写入多长
        int64_t wroteLen = 0;

        // 已经写了多少个文件
        int fileCount = 0;

        // 最后一次读取配置的时间点
        int64_t lastLoadConfigTP = 0;

    public:
        // 参数：开辟多少兆初始内存 cache
        explicit Logger(size_t const &capMB = 8, char const *const &cfgName = nullptr) {
            // 如果有传入新的配置文件名 就覆盖
            if (cfgName) {
                this->cfgName = cfgName;
            }

            // 试着加载配置
            LoadConfig();

            // 打开当前日志文件
            OpenLogFile();

            // 初始化内存池
            items1.reserve(1024 * 1024 * capMB / 256);
            items2.reserve(1024 * 1024 * capMB / 256);

            // 初始化后台线程
            thread = std::thread(&Logger::Loop, this);
        }

        ~Logger() {
            // 可能是后台线程主动异常退出导致
            if (disposing) return;

            // 通知后台线程收尾退出
            disposing = 1;

            // 等后台线程结束
            thread.join();
        }

        // 固定以 level, __LINE__, __FILE__, __FUNCTION__, now 打头的日志内容写入. 宏代指的 char* 不会丢失，故不需要复制其内容
        // 直接写入固定无 type 前缀: level + lineNumber + fileName* + funcName* + now
        template<typename ...TS>
        void
        Log(xx::LogLevels const &level, int const &lineNumber, char const *const &fileName, char const *const &funcName,
            TS const &...vs) {
            // 忽略一些级别不写入
            if ((int) level < cfg.logLevel) return;

            // 如果已经在析构阶段也不写入
            if (disposing) return;

            // 锁定队列
#ifdef XX_LOG_USE_XX_MUTEX
            LockGuard lg(mtx);
#else
            std::lock_guard<std::mutex> lg(mtx);
#endif

            // 申请一条存储空间
            auto &&d = items1.emplace_back();
            // 确保存储空间一定存的下这些参数
            static_assert(Item::innerSpaceLen >= sizeof(int) + sizeof(int) + sizeof(char *) + sizeof(char *) +
                                                 sizeof(std::chrono::system_clock::time_point));
            // 准备开始将参数复制到 data( 这几个 char* 参数可以直接复制指针而不必担心其消失，因为是编译器编译到代码数据段里的 )
            auto p = d.buf;
            *(int *) p = (int) level;
            *(int *) (p + sizeof(int)) = lineNumber;
            *(char const **) (p + sizeof(int) + sizeof(int)) = fileName;
            *(char const **) (p + sizeof(int) + sizeof(int) + sizeof(char *)) = funcName;
            *(std::chrono::system_clock::time_point *) (p + sizeof(int) + sizeof(int) + sizeof(char *) +
                                                        sizeof(char *)) = std::chrono::system_clock::now();
            d.len = sizeof(int) + sizeof(int) + sizeof(char *) + sizeof(char *) +
                    sizeof(std::chrono::system_clock::time_point);
            // 继续常规写入变参部分( typeid + data, typeid + data, ....... )
            xx::WriteTo(d, vs...);
        }

        // 能简单粗略查询队列是否已经写空( 不一定准确, 测试目的 )
        inline bool const &Busy() const {
            return writing;
        }

    protected:
        // 后台线程专用函数
        inline void Loop() {
            while (true) {
                // 每 ? 秒重新读一次配置
                LoadConfig();

                // 如果有数据: 交换前后台队列. 没有就继续循环
                {
#ifdef XX_LOG_USE_XX_MUTEX
                    LockGuard lg(mtx);
#else
                    std::lock_guard<std::mutex> lg(mtx);
#endif
                    if (items1.empty()) goto LabEnd;
                    std::swap(items1, items2);
                }

                // 开始将后台队列的内容写入存储
                {
                    writing = true;
                    // 输出 items2
                    Dump();
                    items2.clear();
                    writing = false;
                    continue;
                }

                LabEnd:
                // 如果需要退出，还是要多循环一次，写光数据
                {
                    if (disposing == 1) {
                        ++disposing;
                        continue;
                    } else if (disposing == 2) break;
                }

                // 省点 cpu
                std::this_thread::sleep_for(std::chrono::milliseconds(loopSleepMS));
            }
        }

        // 文件编号循环使用 & 改名逻辑
        inline void FileRename() {
            ofs.close();

            int i = fileCount;
            char oldName[225], newName[225];
            while (i > 0) {
                snprintf(oldName, sizeof(oldName), "%s.%d", currLogFileName.c_str(), i);
                snprintf(newName, sizeof(newName), "%s.%d", currLogFileName.c_str(), i + 1);
                if (i == cfg.logFileCount) {
                    remove(oldName);
                } else {
                    rename(oldName, newName);
                }
                --i;
            }

            snprintf(oldName, sizeof(oldName), "%s", currLogFileName.c_str());
            snprintf(newName, sizeof(newName), "%s.%d", currLogFileName.c_str(), i + 1);
            rename(oldName, newName);

            if (fileCount < cfg.logFileCount) {
                ++fileCount;
            }

            ofs.open(oldName, std::ios_base::app);
            if (ofs.fail()) {
                std::cerr << "ERROR!!! open log file failed: \"" << oldName << "\", forget mkdir ??" << std::endl;
            }
        }

        inline void LoadConfig() {
            auto &&nowTicks = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            if (nowTicks - lastLoadConfigTP > reloadConfigIntervalSeconds) {
                // 试图加载 logger cfg
                if (std::ifstream(cfgName.c_str()).good()) {
                    ajson::load_from_file(cfg, cfgName.c_str());
#ifdef LOG_COUT_VERBOS
                    std::cout << "logger load \"" << cfgName << "\" = " << cfg << std::endl;
#endif
                } else {
#ifdef LOG_COUT_VERBOS
                    std::cout << "can't find config file: " << cfgName << ", will be use default settings = " << cfg << std::endl;
#endif
                }
                // 更新最后加载时间
                lastLoadConfigTP = nowTicks;
            }
        }

        inline void OpenLogFile() {
            // 根据配置和当前时间推算出日志文件名并以追加模式打开
            currLogFileName = cfg.logFileName + ToString(std::chrono::system_clock::now(), ".%F_%H.%M.%S");
            ofs.open(currLogFileName, std::ios_base::app);
            if (ofs.fail()) {
                std::cerr << "ERROR!!! open log file failed: \"" << currLogFileName << "\", forget mkdir ??"
                          << std::endl;
            }
        }

        // 默认提供文件 dump 和 console dump. 可以覆盖实现自己的特殊需求
        inline virtual void Dump() {
            // 输出到控制台?
            if (cfg.outputConsole) {
                for (auto &&item : items2) {
                    DumpItem(std::cout, item, true);
                }
                std::cout.flush();
            }

            // 输出到文件 & 文件打开成功 ?
            if (ofs.is_open()) {
                for (auto &&item : items2) {
                    DumpItem(ofs, item, false);

                    // 粗略统计当前已经写了多长了. 不是很精确. 如果超长就换文件写
                    wroteLen += item.len;
                    if (wroteLen > cfg.logFileMaxBytes) {
                        wroteLen = 0;
                        FileRename();
                    }
                }
                ofs.flush();
            }
        }

        // dump 单行日志. 可覆盖实现自己的特殊需求
        inline virtual void DumpItem(std::ostream &o, Item &item, bool const &isConsole) {
            // dump 前缀. 反向取出几个头部参数, 传递到格式化函数
            auto p = item.buf;
            Dump_Prefix(o, (LogLevels) *(int *) p, *(int *) (p + sizeof(int)),
                        *(char const **) (p + sizeof(int) + sizeof(int)),
                        *(char const **) (p + sizeof(int) + sizeof(int) + sizeof(char *)),
                        *(std::chrono::system_clock::time_point *) (p + sizeof(int) + sizeof(int) + sizeof(char *) +
                                                                    sizeof(char *)), isConsole);
            // dump 内容
            DumpTo(o, item, sizeof(int) + sizeof(int) + sizeof(char *) + sizeof(char *) +
                            sizeof(std::chrono::system_clock::time_point));
            // 弄个换行符
            o << std::endl;
        }

        // dump 单行日志 的前缀部分。可覆盖实现自己的写入格式
        inline virtual void
        Dump_Prefix(std::ostream &o, LogLevels const &level, int const &lineNumber, char const *const &fileName, char const *const &funcName,
                    std::chrono::system_clock::time_point const &tp, bool const &isConsole) {
            if (lineNumber) {
                if (isConsole) {
                    o << "\033[36m" << ToString(tp) << "\033[37m" << " [" << logLevelColorNames[(int) level] << "] [file:\033[36m" << fileName << "\033[37m line:\033[36m"
                      << lineNumber << "\033[37m func:\033[36m" << funcName << "\033[37m] ";
                } else {
                    o << ToString(tp) << " [" << logLevelNames[(int) level] << "] [file:" << fileName << " line:" << lineNumber << " func:" << funcName << "] ";
                }
            } else {
                // 行号为 0 乃精简模式. 不输出 文件，行号，函数名
                if (isConsole) {
                    o << "\033[36m" << ToString(tp) << "\033[37m" << " [" << logLevelColorNames[(int) level] << "\033[37m] ";
                } else {
                    o << ToString(tp) << " [" << logLevelNames[(int) level] << "] ";
                }
            }
        }
    };
}

inline xx::Logger __xxLogger;
#if defined(LOG_INFO) || defined(LOG_WARN) || defined(LOG_ERROR) || defined(LOG_ERR) || defined(LOG_TRACE) || defined(LOG_DEBUG)
#error
#endif

#ifdef XX_DISABLE_ALL_LOGS
#   define LOG_INFO(...) void()
#   define LOG_WARN(...) void()
#   define LOG_ERROR(...) void()
#   define LOG_ERR(...) void()
#   define LOG_TRACE(...) void()
#   define LOG_DEBUG(...) void()
#   define LOG_SIMPLE(...) void()
#else
#   define LOG_INFO(...) __xxLogger.Log(xx::LogLevels::INFO, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_WARN(...) __xxLogger.Log(xx::LogLevels::WARN, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_ERROR(...) __xxLogger.Log(xx::LogLevels::ERROR, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_ERR(...) __xxLogger.Log(xx::LogLevels::ERROR, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_TRACE(...) __xxLogger.Log(xx::LogLevels::TRACE, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_DEBUG(...) __xxLogger.Log(xx::LogLevels::DEBUG, __LINE__, xx::CutPath(__FILE__), __FUNCTION__, __VA_ARGS__)
#   define LOG_SIMPLE(...) __xxLogger.Log(xx::LogLevels::INFO, 0, "", "", __VA_ARGS__)
#endif
