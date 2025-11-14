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

std::tm Task::getDueDate() const
{
    const std::time_t timeValue = std::chrono::system_clock::to_time_t(m_dueDate);

    std::tm timeInfo{};
    localtime_s(&timeInfo, &timeValue); // Versión segura

    return timeInfo;
}

// Returns date and time using ctime
std::string Task::showDueDate() const
{
    std::time_t timeValue = std::chrono::system_clock::to_time_t(m_dueDate);
    char buffer[26]; // ctime_s needs a buffer of at least 26 characters
    ctime_s(buffer, sizeof(buffer), &timeValue);

    return buffer;
}

std::ostream& operator<<(std::ostream& output, const Task& task)
{
    output << "Task [" << task.getId() << "]: " << task.getTitle() << ", " << task.showDueDate();
    return output;
}