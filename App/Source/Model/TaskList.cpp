#include "TaskList.h"
#include "Utilities.h"

#include <iostream>
#include <ranges>
#include <optional>
#include <vector>

std::optional<TaskSpecification> TaskList::getTaskSpecification(const int id) const
{
    auto iterator = std::ranges::find(m_list, id, &Task::getId);

    if (iterator != m_list.end())
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
    Task newTask(taskSpecification);

    // Saves the task into the database and storages the id of the new task
    std::optional<int> newId = m_taskManager->createTask(taskSpecification).value();

    if (newId.has_value())
    {
        newTask.setId(newId.value());
        m_list.emplace_back(newTask);
    }
    else
    {
        std::cout << "Error saving task in database\n";
    }
}

void TaskList::deleteTask(const int id) {
    auto iterator = std::ranges::find(m_list, id, &Task::getId);

    if (iterator != m_list.end())
    {
        iterator->setIsDeleted(true);
        m_taskManager->deleteTask(id);
    }
    else
    {
        std::cout << "Task not found\n";
    }
}

void TaskList::editTask(const TaskSpecification& newTaskSpecification)
{
    auto iterator = std::ranges::find(m_list, newTaskSpecification.Id, &Task::getId);

    if (iterator != m_list.end())
    {
        iterator->setTitle(newTaskSpecification.Title);
        iterator->setDescription(newTaskSpecification.Description);
        iterator->setStatus(newTaskSpecification.Status);
        iterator->setDueDate(newTaskSpecification.DueDate);
        m_taskManager->updateTask(newTaskSpecification);
    }
    else
    {
        std::cout << "Task not found\n";
    }
}

void TaskList::show() const
{
    for (const auto& task : m_list)
    {
        if (!task.getIsDeleted()) { std::cout << task << "\n"; }
    }
}

std::vector<TaskSpecification> TaskList::search(const std::string& searchTerm) const
{
    std::vector<TaskSpecification> searchResults{};
    std::string lowerSearchTerm = TaskUtilities::toLower(searchTerm);

    if (lowerSearchTerm.empty())
    {
        return searchResults;
    }

    for (const Task& task : m_list)
    {
        std::string toLowerTitle = TaskUtilities::toLower(task.getTitle());
        std::string toLowerDescription = TaskUtilities::toLower(task.getDescription());

        if (toLowerTitle.find(lowerSearchTerm) != std::string::npos ||
            toLowerDescription.find(lowerSearchTerm) != std::string::npos)
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

void TaskList::deleteList()
{
    m_isDeleted = true;
}
