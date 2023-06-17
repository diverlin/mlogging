#include "currentdatetimeutc.hpp"

#include <chrono>

#include <iostream>
#include <iomanip>

CurrentDateTimeUTC::CurrentDateTimeUTC()
{
    // get the current system time in UTC
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTimet = std::chrono::system_clock::to_time_t(currentTime);
    std::tm* currentTm = std::gmtime(&currentTimet);

    // get the milliseconds
    auto duration = currentTime.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;

    // date
    std::ostringstream oss;
    oss << std::put_time(currentTm, "%Y-%m-%d");
    m_dateStr = oss.str();

    // time
    oss.str("");
    oss << std::put_time(currentTm, "%H:%M:%S") << ".";
    oss << std::setfill('0') << std::setw(3) << milliseconds.count();
    m_timeStr = oss.str();

    m_dateTimeStr = m_dateStr + " " + m_timeStr;
}
