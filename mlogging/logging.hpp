#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unordered_map>
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
    Logging(const std::string& rootPath="");
    ~Logging();

    void setRootPath(const std::string& rootPath) { m_rootPath = rootPath; }
    void log(const std::string& msg, const std::string& locationPattern="orders");

private:
    std::string m_rootPath;
    std::vector<std::thread> m_threads;
    std::unordered_map<std::string, std::queue<std::string>> m_locationPattern2LogQueueMap;
    std::unordered_map<std::string, std::string> m_locationPattern2FilePathMap;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_isStopThreadsRequested = false;

    void threadWork(const std::string& locationPattern);

    static std::string filePath(const std::string& rootPath, const std::string& locationPattern, const CurrentDateTimeUTC& dt);
    static void reopenFilePath(std::ofstream&, std::string& filePath);
};

} // namespace custom

