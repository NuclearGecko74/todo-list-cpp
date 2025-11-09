#include "Task.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

void Task::setDueDate(int year, int month, int day, int hour, int minute, int second)
{
    std::tm timeStruct = {};
    timeStruct.tm_year = year - 1900; // years since 1900
    timeStruct.tm_mon = month - 1;    // 0 = january
    timeStruct.tm_mday = day;
    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    std::time_t timeValue = std::mktime(&timeStruct);
    m_dueDate = std::chrono::system_clock::from_time_t(timeValue);
}

std::string Task::getCreateDate() const
{
    // Get the current calendar time as a time_t object
    const std::time_t now = std::time(nullptr);

    // Convert the time_t object to a human-readable string
    const char* date_time_str = std::ctime(&now);
    return date_time_str;
}

// Returns date and time using ctime
std::string Task::getDueDate() const
{
    std::time_t timeValue = std::chrono::system_clock::to_time_t(m_dueDate);
    char buffer[26]; // ctime_s needs a buffer of at least 26 characters
    ctime_s(buffer, sizeof(buffer), &timeValue);

    return buffer;
}