#include "logging.hpp"

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

void Logging::threadWork(const std::string& category)
{
    std::string filename = "log" + category + ".txt";
    std::ofstream file(filename, std::ios::app);

    std::unique_lock<std::mutex> lock(m_mutex);
    while (true) {
        m_conditionVariable.wait(lock, [this, category]() {
            return !m_category2LogQueueMap[category].empty() || m_isStopThreadsRequested;
        });

        if (m_isStopThreadsRequested) {
            break;
        }

        // process all log messages in the queue belonging to current category
        std::queue<std::string>& queuePerCategory = m_category2LogQueueMap[category];
        while (!queuePerCategory.empty()) {
            std::string msg = queuePerCategory.front();
            queuePerCategory.pop();
            MLOGGER_DEBUG("th", std::this_thread::get_id(), "write", msg, "category=", category);

            // write the log message to the file
            file << msg << std::endl;
        }

        file.flush(); // flush the output to ensure it's written immediately
    }
    lock.unlock();

    file.close();
}

} // namespace custom
