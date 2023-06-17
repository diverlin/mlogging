#include "logging.hpp"
#include "currentdatetimeutc.hpp"

#include <iostream>
#include <fstream>

namespace custom {

Logging::Logging()
{
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

void Logging::log(const std::string& msg, const std::string& category)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    MLOGGER_DEBUG("add log queue", msg, category);
    const bool isThreadForCategoryExists = m_category2LogQueueMap.find(category) != m_category2LogQueueMap.end();
    m_category2LogQueueMap[category].push(msg);
    if (!isThreadForCategoryExists) {
        m_threads.emplace_back([this, category]() { threadWork(category); });
        MLOGGER_DEBUG("create new thread num=", m_threads.size());
    } else {
        // if thread exists, we wake up all sleeping threads, where the only thread which has messages queue will continue it's work
        m_conditionVariable.notify_all();
    }
}

std::string Logging::fileName(const CurrentDateTimeUTC& dt, const std::string& category)
{
    std::string result(dt.dateStr());
    result += "_" + category;
    return result;
}

void Logging::threadWork(const std::string& category)
{
    CurrentDateTimeUTC startDt;
    std::string startFileName = Logging::fileName(startDt, category);
    std::ofstream file(startFileName, std::ios::app);

    std::unique_lock<std::mutex> lock(m_mutex);
    m_category2FileNameMap[category] = startFileName;
    while (true) {
        m_conditionVariable.wait(lock, [this, category]() {
            return !m_category2LogQueueMap[category].empty() || m_isStopThreadsRequested;
        });

        if (m_isStopThreadsRequested) {
            break;
        }

        CurrentDateTimeUTC dt;
        std::string currentFileName = Logging::fileName(dt, category);
        if (currentFileName != m_category2FileNameMap[category]) {
            file.close();
            file.open(currentFileName, std::ios::app);
            m_category2FileNameMap[category] = currentFileName;
        }

        // process all log messages in the queue belonging to current category
        std::queue<std::string>& queuePerCategory = m_category2LogQueueMap[category];
        while (!queuePerCategory.empty()) {
            std::string msg = queuePerCategory.front();
            queuePerCategory.pop();
            // write the log message to the file
            file << dt.dateTimeStr() << "," << std::this_thread::get_id() << ":" << msg << std::endl;
            MLOGGER_DEBUG("th", std::this_thread::get_id(), "write", msg, "category=", category);
        }

        file.flush(); // flush the output to ensure it's written immediately
    }
    lock.unlock();

    file.close();
}

} // namespace custom
