#include "logging.hpp"
#include "currentdatetimeutc.hpp"
#include "fsutils.hpp"

#include <iostream>
#include <fstream>

namespace custom {

Logging::Logging()
{
    //fsutils::run_tests();
}

Logging::~Logging()
{
    {
        // before joining thread we need unsleep them and finish they bodies.
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isStopThreadsRequested = true; // condition to unsleep
        m_conditionVariable.notify_all(); // wakeup all threads
    }

    for (std::thread& thread: m_threads) {
        thread.join();
    }
}

void Logging::log(const std::string& msg, const std::string& locationPattern)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    MLOGGER_DEBUG("add log queue", msg, locationPattern);
    const bool isThreadForLocationPatternExists = m_locationPattern2LogQueueMap.find(locationPattern) != m_locationPattern2LogQueueMap.end();
    m_locationPattern2LogQueueMap[locationPattern].push(msg);
    if (!isThreadForLocationPatternExists) {
        m_threads.emplace_back([this, locationPattern]() { threadWork(locationPattern); });
        MLOGGER_DEBUG("create new thread num=", m_threads.size());
    } else {
        // if thread exists, we wake up all sleeping threads, where the only thread which has messages queue will continue it's work
        m_conditionVariable.notify_all();
    }
}

std::string Logging::filePath(const CurrentDateTimeUTC& dt, const std::string& locationPattern)
{
    std::string basePath = fsutils::basePath(locationPattern);
    std::string baseName = fsutils::baseName(locationPattern);
    return basePath + dt.dateStr() + "_" + baseName;
}

void Logging::threadWork(const std::string& locationPattern)
{
    CurrentDateTimeUTC startDt;
    std::string startFilePath = Logging::filePath(startDt, locationPattern);
    std::ofstream file;
    reopenFilePath(file, startFilePath);

    std::unique_lock<std::mutex> lock(m_mutex);
    m_locationPattern2FilePathMap[locationPattern] = startFilePath;
    while (true) {
        m_conditionVariable.wait(lock, [this, locationPattern]() {
            return !m_locationPattern2LogQueueMap[locationPattern].empty() || m_isStopThreadsRequested;
        });

        if (m_isStopThreadsRequested) {
            break;
        }

        CurrentDateTimeUTC dt;
        std::string currentFilePath = Logging::filePath(dt, locationPattern);
        if (currentFilePath != m_locationPattern2FilePathMap[locationPattern]) { // the currentFilePath take into account current date(day num) which will trigger new file creation
            reopenFilePath(file, currentFilePath);
            m_locationPattern2FilePathMap[locationPattern] = currentFilePath;
        }

        // process all log messages in the queue belonging to current locationPattern
        std::queue<std::string>& queues = m_locationPattern2LogQueueMap[locationPattern];
        while (!queues.empty()) {
            std::string msg = queues.front();
            queues.pop();
            // write the log message to the file
            file << dt.dateTimeStr() << "," << std::this_thread::get_id() << ":" << msg << std::endl;
            MLOGGER_DEBUG("th", std::this_thread::get_id(), "write", msg, "locationPattern=", locationPattern);
        }

        file.flush(); // flush the output to ensure it's written immediately
    }
    lock.unlock();

    file.close();
}

void Logging::reopenFilePath(std::ofstream& file, std::string& filePath)
{
    if (file.is_open()) {
        file.close();
    }
    std::string basePath = fsutils::basePath(filePath);
    if (!fsutils::isPathExist(basePath)) {
        fsutils::makePath(basePath);
    } else {
        MLOGGER_DEBUG("fail to create path", basePath);
    }
    file.open(filePath, std::ios::app);
}

} // namespace custom
