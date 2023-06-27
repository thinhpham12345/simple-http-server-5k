#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <sstream>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    static Logger &getInstance()
    {
        static Logger instance;
        return instance;
    }

    template <typename T>
    Logger &operator<<(const T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // std::cout << value;
        return *this;
    }

private:
    Logger() {}

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&time);
        timestamp.pop_back(); // Remove trailing newline
        return timestamp;
    }

    std::mutex mutex_;
};

inline std::string getLogLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:
        return "[INFO]";
    case LogLevel::WARNING:
        return "[WARNING]";
    case LogLevel::ERROR:
        return "[ERROR]";
    }
    return "";
}

#define log(level) Logger::getInstance() << getLogLevel(LogLevel::level)

#endif
