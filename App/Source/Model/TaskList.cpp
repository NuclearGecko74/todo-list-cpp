#include "TaskList.h"

TaskList::TaskList(const std::string& name, const Task& task)
{
    m_name = name;
    m_List.emplace_back(task);
}

TaskList::TaskList(const std::string &name, const std::string &description, const Task& task)
{
    m_name = name;
    m_description = description;
    m_List.emplace_back(task);
}