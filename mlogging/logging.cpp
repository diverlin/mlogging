#include "logging.hpp"
#include "currentdatetimeutc.hpp"
#include "fsutils.hpp"

#include <iostream>
#include <fstream>

namespace custom {

Logging::Logging(const std::string& rootPath)
    : m_rootPath(rootPath)
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

void Logging::handleLogMessage(const std::string& locationPattern, const std::string& msg)
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

void Logging::threadWork(const std::string& locationPattern)
{
    CurrentDateTimeUTC startDt;
    std::ofstream file;

    std::unique_lock<std::mutex> lock(m_mutex);

    std::string startFilePath = Logging::filePath(m_rootPath, locationPattern, startDt);
    reopenFilePath(file, startFilePath);

    m_locationPattern2FilePathMap[locationPattern] = startFilePath;
    while (true) {
        m_conditionVariable.wait(lock, [this, locationPattern]() {
            return !m_locationPattern2LogQueueMap[locationPattern].empty() || m_isStopThreadsRequested;
        });

        if (m_isStopThreadsRequested) {
            break;
        }

        CurrentDateTimeUTC dt;
        std::string currentFilePath = Logging::filePath(m_rootPath, locationPattern, dt);
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
            MLOGGER_DEBUG("th", std::this_thread::get_id(), "write", msg, "locationPattern=", locationPattern, "to", currentFilePath);
        }

        file.flush(); // flush the output to ensure it's written immediately
    }
    lock.unlock();

    file.close();
}

std::string Logging::filePath(const std::string& rootPath, const std::string& locationPattern, const CurrentDateTimeUTC& dt)
{
    std::string basePath = fsutils::basePath(locationPattern);
    std::string baseName = fsutils::baseName(locationPattern);
    std::string result = basePath + dt.dateStr() + "_" + baseName;
    if (!rootPath.empty()) {
        result = rootPath + "/" + result + ".log";
    }
    return result;
}

void Logging::reopenFilePath(std::ofstream& file, std::string& filePath)
{
    if (file.is_open()) {
        file.close();
    }
    std::string basePath = fsutils::basePath(filePath);
    if (!fsutils::isPathExist(basePath)) {
        MLOGGER_DEBUG("create path", basePath);
        if (!fsutils::makePath(basePath)) {
            MLOGGER_DEBUG("fail to create path", basePath);
        }
    }
    file.open(filePath, std::ios::app);
}

} // namespace custom
