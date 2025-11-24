#ifndef TODOLIST_TASKMANAGER_H
#define TODOLIST_TASKMANAGER_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "Task.h"

class TaskManager
{
private:
    SQLite::Database& m_db;
public:
    explicit TaskManager(SQLite::Database& database) : m_db(database) {}

    std::optional<int> createTask(const TaskSpecification& specification);

    std::vector<Task> loadTasks(const int listId);

    bool updateTask(const TaskSpecification& specification);

    bool deleteTask(const int taskId);
};

#endif //TODOLIST_TASKMANAGER_H