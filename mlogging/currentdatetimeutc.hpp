#pragma once

#include <string>

class CurrentDateTimeUTC {
public:
    CurrentDateTimeUTC();
    ~CurrentDateTimeUTC()=default;

    const std::string& dateStr() const { return m_dateStr; }
    const std::string& timeStr() const { return m_timeStr; }
    const std::string& dateTimeStr() const { return m_dateTimeStr; }

private:
    std::string m_dateStr;
    std::string m_timeStr;
    std::string m_dateTimeStr;
};

