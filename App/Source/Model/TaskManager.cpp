#include "TaskManager.h"
#include "Utilities.h"
#include <SQLiteCpp/SQLiteCpp.h>

std::optional<int> TaskManager::createTask(const TaskSpecification &specification)
{
    SQLite::Statement query(m_db,
        "INSERT into task (title, description, isDeleted, status, createDate, dueDate, listId) VALUES (?,?,?,?,DATETIME('now'),?,?)");

    query.bind(1, specification.Title);
    query.bind(2, specification.Description);
    query.bind(3, specification.isDeleted);
    query.bind(4, specification.Status);
    query.bind(5, TaskUtilities::timePointToString(specification.DueDate));
    query.bind(6,specification.ListId);

    if (query.exec()) { return static_cast<int>(m_db.getLastInsertRowid()); }
    return std::nullopt;
}

std::vector<Task> TaskManager::loadTasks(const int listId)
{
    std::vector<Task> list;
    SQLite::Statement query(m_db,
        "SELECT taskId, title, description, status, createDate, dueDate FROM task WHERE listId = ? AND isDeleted = 0");

    query.bind(1, listId);

    while (query.executeStep())
    {
        TaskSpecification taskFound;
        taskFound.Id = query.getColumn(0);
        taskFound.Title = query.getColumn(1).getString();
        taskFound.Description = query.getColumn(2).getString();
        taskFound.isDeleted = false;
        taskFound.Status = query.getColumn(3).getInt();
        taskFound.DueDate = TaskUtilities::stringToTimePoint(query.getColumn(4).getString());
        taskFound.ListId = listId;

        list.emplace_back(taskFound);
    }

    return list;
}

bool TaskManager::updateTask(const TaskSpecification &specification)
{
    try
    {
        SQLite::Statement query(m_db,
           "UPDATE task SET title = ?, description = ?, status = ?, dueDate = ? WHERE taskId = ?");

        query.bind(1, specification.Title);
        query.bind(2, specification.Description);
        query.bind(3, specification.Status);
        query.bind(4,TaskUtilities::timePointToString(specification.DueDate));
        query.bind(5, specification.Id);

        query.exec();
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}

bool TaskManager::deleteTask(const int taskId)
{
    try
    {
        SQLite::Statement query(m_db,
            "UPDATE task SET isDeleted = 1 WHERE taskId = ?");

        query.bind(1, taskId);

        query.executeStep();
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}
