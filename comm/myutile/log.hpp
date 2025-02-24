#pragma once
#include "nocopy.hpp"
#include <iostream>
#include <string>
#include "timestamp.hpp"

namespace ns_log {

// 日志级别枚举
enum LogLevels
{
    INFO,  // 普通信息  
    DEBUG, // 调试信息
    ERROR, // 错误信息
    FATAL, // 致命信息
};

// 日志类
class Logger : public nocopy
{
public:
    static Logger& GetInstance()
    {
        static Logger log;
        return log;
    }

    ~Logger() {}

    // 设置日志级别
    void SetLevel(int level)
    {
        logLevel_ = level;
    }

    // 写日志
    void writeLog(std::string message)
    {
        switch (logLevel_)
        {
        case INFO:
            std::cout << "[INFO] ";
            break;
        case ERROR:
            std::cout << "[ERROR] ";
            break;
        case DEBUG:
            std::cout << "[DEBUG] ";
            break;
        case FATAL:
            std::cout << "[FATAL] ";
            break;
        default:
            break;
        }
        std::string time = Timestamp::now().TimestamptoString();
        std::cout << "[" << time.c_str() << "]" << ":" << message.c_str() << std::endl;
    }

private:
    Logger() : logLevel_(INFO) {}

private:
    int logLevel_;
};

} // namespace ns_log

// 宏定义：日志宏，方便调用
#define LOG_INFO(Logmsgformat, ...) \
    do { \
        ns_log::Logger& logger = ns_log::Logger::GetInstance(); \
        logger.SetLevel(ns_log::INFO); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.writeLog(buf); \
    } while (0);

#define LOG_FATAL(Logmsgformat, ...) \
    do { \
        ns_log::Logger& logger = ns_log::Logger::GetInstance(); \
        logger.SetLevel(ns_log::FATAL); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.writeLog(buf); \
        exit(1); \
    } while (0);

#ifdef DEBUGMOD
#define LOG_DEBUF(Logmsgformat, ...) \
    do { \
        ns_log::Logger& logger = ns_log::Logger::GetInstance(); \
        logger.SetLevel(ns_log::DEBUG); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.writeLog(buf); \
    } while (0);
#else
#define LOG_DEBUF(Logmsgformat, ...)
#endif

#define LOG_ERROR(Logmsgformat, ...) \
    do { \
        ns_log::Logger& logger = ns_log::Logger::GetInstance(); \
        logger.SetLevel(ns_log::ERROR); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.writeLog(buf); \
    } while (0);
