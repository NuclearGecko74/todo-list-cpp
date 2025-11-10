#include "Task.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

void Task::setDueDate(std::tm timeStruct)
{
    std::time_t timeValue = std::mktime(&timeStruct);
    m_dueDate = std::chrono::system_clock::from_time_t(timeValue);
}

// Returns date and time using ctime
std::string Task::getDueDate() const
{
    std::time_t timeValue = std::chrono::system_clock::to_time_t(m_dueDate);
    char buffer[26]; // ctime_s needs a buffer of at least 26 characters
    ctime_s(buffer, sizeof(buffer), &timeValue);

    return buffer;
}