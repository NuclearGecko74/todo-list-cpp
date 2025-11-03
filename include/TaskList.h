#ifndef TODOLIST_TASKLIST_H
#define TODOLIST_TASKLIST_H

#include <iostream>
#include <vector>
#include "Task.h"

class TaskList {
private:
    std::string m_listName;
    short m_size = 0;
    std::vector<Task> m_List;

public:
    // Constructors
    TaskList() = delete;

    explicit TaskList(std::string name)
        : m_listName((std::move(name) )) {};

    explicit TaskList(std::string name, Task task);

    //
};

#endif //TODOLIST_TASKLIST_H