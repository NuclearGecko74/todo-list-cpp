#include "TaskList.h"

TaskList::TaskList(const int id, const std::string& name, const Task& task, const int userId)
    : m_id(id), m_name(name), m_userId(userId)
{
    m_List.emplace_back(task);
}

TaskList::TaskList(const int id, const std::string& name, const std::string& description, const Task& task, const int userId)
    : m_id(id), m_name(name), m_description(description), m_userId(userId)
{
    m_List.emplace_back(task);
}

std::string TaskList::getCreateDate() const
{
    // Get the current calendar time as a time_t object
    const std::time_t now = std::time(nullptr);

    // Convert the time_t object to a human-readable string
    const char* date_time_str = std::ctime(&now);
    return date_time_str;
}

void TaskList::addTask(const TaskSpecification& taskSpecification)
{
    Task newTask(taskSpecification.Id, taskSpecification.Name, taskSpecification.Description);

    newTask.setStatus(taskSpecification.Status);
    newTask.setDueDate(taskSpecification.DueDate);

    m_List.emplace_back(newTask);
}

void TaskList::deleteTask(const int id)
{
    auto refTaskDeleted = std::ranges::find(m_List, id, &Task::getId);

    if (refTaskDeleted != m_List.end())
    {
        refTaskDeleted->setIsDeleted(true);
    }
    else
    {
        std::cout << "Task not found\n";
    }
}
