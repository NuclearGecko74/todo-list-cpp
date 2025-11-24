#include "ListManager.h"
#include "Utilities.h"
#include "SQLiteCpp/SQLiteCpp.h"

std::optional<int> ListManager::createList(const TaskListSpecification& specification)
{
    SQLite::Statement query(m_Db,
        "INSERT into list (name, description, createDate, userId, isDeleted) VALUES(?,?,DATETIME('now'),?,?)" );

    query.bind(1, specification.Name);
    query.bind(2, specification.Description);
    query.bind(3, specification.UserId);
    query.bind(4, specification.IsDeleted);

    if (query.exec()) { return static_cast<int>(m_Db.getLastInsertRowid()); }

    return std::nullopt;
}

std::vector<TaskListSpecification> ListManager::loadLists(const int userId)
{
    std::vector<TaskListSpecification> userLists;
    SQLite::Statement query(m_Db,
        "SELECT listId, name, description, createDate FROM list WHERE userId = ? AND isDeleted = 0" );

    query.bind(1, userId);

    while (query.executeStep())
    {
        TaskListSpecification listFound;
        listFound.Id = query.getColumn(0);
        listFound.Name = query.getColumn(1).getString();
        listFound.Description = query.getColumn(2).getString();
        listFound.CreateDate = TaskUtilities::stringToTimePoint(
            query.getColumn(3).getString());
        listFound.UserId = userId;

        userLists.push_back(listFound);
    }

    return userLists;
}

std::optional<TaskListSpecification> ListManager::getList(const int listId)
{
    SQLite::Statement query(m_Db,
        "SELECT listId, name, description, createDate, userID FROM list WHERE listId = ? AND isDeleted = 0" );

    query.bind(1, listId);

    if (query.executeStep())
    {
        TaskListSpecification foundList;
        foundList.Id = query.getColumn(0);
        foundList.Name = query.getColumn(1).getString();
        foundList.Description = query.getColumn(2).getString();
        foundList.CreateDate = TaskUtilities::stringToTimePoint(
            query.getColumn(3).getString());
        foundList.UserId = query.getColumn(4);

        return foundList;
    }

    return std::nullopt;
}

bool ListManager::updateList(const TaskListSpecification &specification)
{
    try
    {
        SQLite::Statement query(m_Db,
           "UPDATE list SET name = ?, description = ? WHERE listId = ?");

        query.bind(1, specification.Name);
        query.bind(2, specification.Description);
        query.bind(3, specification.Id);

        query.executeStep();
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}

bool ListManager::deleteList(const int listId)
{
    try
    {
        SQLite::Statement query(m_Db,
           "UPDATE list SET isDeleted = 1 WHERE listId = ?");

        query.bind(1, listId);

        query.executeStep();
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}

