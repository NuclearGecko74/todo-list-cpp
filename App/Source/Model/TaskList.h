#ifndef TODOLIST_TASKLIST_H
#define TODOLIST_TASKLIST_H

#include <vector>
#include "Task.h"

class TaskList {
private:
    std::string m_name;
    std::string m_description;
    std::vector<Task> m_List;

public:
    // constructors
    TaskList() = delete;

    explicit TaskList(const std::string& name)
        : m_name(name) {};

    explicit TaskList(const std::string& name, const std::string& description)
        :  m_name(name), m_description(description) {};

    explicit TaskList(const std::string& name, const Task& task);

    explicit TaskList(const std::string& name, const std::string& description, const Task& task);

    // setters
    inline void setName(const std::string& name) { m_name = name; }

    // getters
    [[nodiscard]] inline std::string getName() const { return m_name; }
    [[nodiscard]] inline size_t getSize() const { return m_List.size(); }
};

#endif