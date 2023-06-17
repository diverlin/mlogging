#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <queue>

class CurrentDateTimeUTC;

#ifdef ENABLE_MLOGGER_DEBUG

#define MLOGGER_DEBUG(...) PrintHelper(__VA_ARGS__)

#include <iostream>
template<typename T>
void PrintHelper(T arg)
{
    std::cout << arg << std::endl;
}

template<typename T, typename... Args>
void PrintHelper(T arg, Args... args)
{
    std::cout << arg << " ";
    PrintHelper(args...);
}
#else

#define MLOGGER_DEBUG(...)

#endif

namespace custom {

class Logging {
public:
    Logging();
    ~Logging();

    void log(const std::string& msg, const std::string& category="default");

private:
    std::vector<std::thread> m_threads;
    std::map<std::string, std::queue<std::string>> m_category2LogQueueMap;
    std::map<std::string, std::string> m_category2FileNameMap;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_isStopThreadsRequested = false;

    void threadWork(const std::string& category);

    static std::string fileName(const CurrentDateTimeUTC& dt, const std::string& category);
};

} // namespace custom

