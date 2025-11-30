#ifndef TODOLIST_LISTREPOSITORY_H
#define TODOLIST_LISTREPOSITORY_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "TaskList.h"

#include <optional>
#include <vector>

class ListManager
{
private:
    SQLite::Database& m_db;

public:
    explicit ListManager(SQLite::Database& database) : m_db(database) {}

    std::optional<int> createList(const TaskListSpecification& specification);

    std::vector<TaskListSpecification> loadLists(const int userId);

    std::optional<TaskListSpecification> getList(const int listId);

    bool updateList(const TaskListSpecification& specification);

    bool deleteList(const int listId);
};

#endif //TODOLIST_LISTREPOSITORY_H