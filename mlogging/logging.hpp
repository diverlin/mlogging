#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <queue>

namespace custom {

class Logging {
public:
    Logging();
    ~Logging();

    void log(const std::string& msg, const std::string& category="default");

private:
    std::vector<std::thread> m_threads;
    std::map<std::string, bool> m_wakeUpFlags;
//    std::map<std::string, std::vector<std::string>> m_queues;
    std::map<std::string, std::queue<std::string>> m_logQueue;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_isStopThreadsRequested = false;

//    void wakeUpThread(const std::string& category);
    void doWork(const std::string& category);
};

} // namespace custom

