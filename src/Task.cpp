#include "Task.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

//Establece el titulo
void Task::setTitle(std::string newTitle)
{
    title = newTitle;
}

//Retorna el titulo en forma de string
std::string Task::getTitle()
{
    return title;
}

//Establece el texto
void Task::setDescription(std::string newDescription)
{
    description = newDescription;
}

//Retorna el texto en forma de string (WIP Propenso a cambiar)
std::string Task::getDescription()
{
    return description;
}

//Establece el estado
void Task::setStatus(bool newStatus)
{
    statusComplete = newStatus;
}

//Retorna el estado en forma de bool (false = en progreso, true = completada)
bool Task::getStatus()
{
    return statusComplete;
}

void Task::setDueDate(int year, int month, int day, int hour, int minute, int second)
{
    std::tm timeStruct = {};
    timeStruct.tm_year = year - 1900; // años desde 1900
    timeStruct.tm_mon = month - 1;    // 0 = enero
    timeStruct.tm_mday = day;
    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    std::time_t timeValue = std::mktime(&timeStruct);
    dueDate = std::chrono::system_clock::from_time_t(timeValue);
}

// Muestra la fecha y hora usando ctime
std::string Task::getDueDate() const
{
    std::time_t timeValue = std::chrono::system_clock::to_time_t(dueDate);
    char buffer[26]; // ctime_s necesita un buffer de al menos 26 caracteres
    ctime_s(buffer, sizeof(buffer), &timeValue);

    return buffer;
}