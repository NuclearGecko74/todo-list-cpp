#include "TaskList.h"

std::string TaskList::toLower(std::string text)
{
    for (auto& character : text)
    {
        character = std::tolower(static_cast<unsigned char>(character));
    }

    return text;
}

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

std::optional<TaskSpecification> TaskList::getTaskSpecification(const int id) const
{
    auto iterator = std::ranges::find(m_List, id, &Task::getId);

    if (iterator != m_List.end())
    {
        TaskSpecification taskSpecification;

        taskSpecification.Id = iterator->getId();
        taskSpecification.Title = iterator->getTitle();
        taskSpecification.Description = iterator->getDescription();
        taskSpecification.Status = iterator->getStatus();
        taskSpecification.DueDate = iterator->getDueDate();

        return taskSpecification;
    }

    return std::nullopt;
}

void TaskList::addTask(const TaskSpecification& taskSpecification)
{
    Task newTask(taskSpecification.Id, taskSpecification.Title, taskSpecification.Description);

    newTask.setStatus(taskSpecification.Status);
    newTask.setDueDate(taskSpecification.DueDate);

    m_List.emplace_back(newTask);
}

void TaskList::deleteTask(const int id)
{
    auto iterator = std::ranges::find(m_List, id, &Task::getId);

    if (iterator != m_List.end())
    {
        iterator->setIsDeleted(true);
    }
    else
    {
        std::cout << "Task not found\n";
    }
}

void TaskList::editTask(const TaskSpecification& newTaskSpecification)
{
    auto iterator = std::ranges::find(m_List, newTaskSpecification.Id, &Task::getId);

    if (iterator != m_List.end())
    {
        iterator->setTitle(newTaskSpecification.Title);
        iterator->setDescription(newTaskSpecification.Description);
        iterator->setStatus(newTaskSpecification.Status);
        iterator->setDueDate(newTaskSpecification.DueDate);
    }
    else
    {
        std::cout << "Task not found\n";
    }
}

void TaskList::show() const
{
    for (const auto& task : m_List)
    {
        if (!task.getIsDeleted()) { std::cout << task << "\n"; }
    }
}

std::vector<TaskSpecification> TaskList::search(const std::string& searchTerm)
{
    std::vector<TaskSpecification> searchResults{};
    std::string lowerSearchTerm = toLower(searchTerm);

    if (lowerSearchTerm.empty())
    {
        return searchResults;
    }

    for (const Task& task : m_List)
    {
        std::string toLowerTitle = toLower(task.getTitle());
        std::string toLowerDescription = toLower(task.getDescription());

        if (toLowerTitle.find(searchTerm) != std::string::npos ||
            toLowerDescription.find(searchTerm) != std::string::npos)
        {
            TaskSpecification taskFound;
            taskFound.Id = task.getId();
            taskFound.Title = task.getTitle();
            taskFound.Description = task.getDescription();
            taskFound.DueDate = task.getDueDate();
            taskFound.Status = task.getStatus();

            searchResults.push_back(taskFound);
        }
    }

    return searchResults;
}
