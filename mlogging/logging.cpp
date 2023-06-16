#include "logging.hpp"

#include <iostream>
#include <fstream>

namespace custom {

Logging::Logging()
{
}

Logging::~Logging()
{
    std::cout << "1 Logging::finish" << std::endl;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isStopThreadsRequested = true;
        m_conditionVariable.notify_all();
    }
    std::cout << "2 Logging::finish" << std::endl;
    for (std::thread& thread: m_threads) {
        auto thread_id = thread.get_id();
        std::cout << "join thread" << thread_id << std::endl;
        thread.join();
        std::cout << "finish join thread" << thread_id << std::endl;
    }
}

void Logging::log(const std::string& msg, const std::string& category)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "add log queue " << msg << " " << category << std::endl;
    m_logQueue[category].push(msg);
    bool isThreadExists = m_wakeUpFlags.find(category) != m_wakeUpFlags.end();
    if (!isThreadExists) {
        m_wakeUpFlags[category] = true;
        m_threads.emplace_back([this, category]() { doWork(category); });
        std::cout << "create new thread" << m_threads.size() << std::endl;
    } else {
        m_wakeUpFlags[category] = true;
        m_conditionVariable.notify_all();
    }
}

void Logging::doWork(const std::string& category)
{
    std::string filename = "log" + category + ".txt";
    std::ofstream file(filename, std::ios::app);

    //std::cout << "doWork thread" << std::this_thread::get_id() << std::endl;
    while (true) {
            std::unique_lock<std::mutex> lock(m_mutex);
        //        std::cout << "---inside loop doWork thread" << std::this_thread::get_id() << std::endl;
        m_conditionVariable.wait(lock, [this, category]() {
            return m_wakeUpFlags[category] || m_isStopThreadsRequested;
        });

        if (m_isStopThreadsRequested) {
            break;
        }

        // Thread-specific work
//        for (const std::string& msg: m_queues[category]) {
//            std::cout << "thread id=" << std::this_thread::get_id() << "msg=" << msg << " category=" << category << " is awake!" << std::endl;
//        }

        // Process the next log message in the queue
        std::queue<std::string>& queuePerCategory = m_logQueue[category];
        while (!queuePerCategory.empty()) {
            std::string msg = queuePerCategory.front();
            queuePerCategory.pop();
            std::cout << "th" << std::this_thread::get_id() << " write " << msg << " category=" << category << std::endl;

            // Write the log message to the file
            file << "File " << msg << std::endl;
        }

        file.flush(); // Flush the output to ensure it's written immediately

        // Reset the flag
        m_wakeUpFlags[category] = false;

        lock.unlock();
    }

    file.close();
}

} // namespace custom
