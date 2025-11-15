#ifndef TODOLIST_TASKLIST_H
#define TODOLIST_TASKLIST_H

#include <iostream>
#include <vector>
#include "Task.h"

struct TaskSpecification{
    int Id = -1;
    std::string Title = "Unnamed Task";
    std::string Description;
    bool Status = false;
    std::tm DueDate = {0,0,0,0,0,0};

    bool operator==(const TaskSpecification& other) const { return Id == other.Id; }
    bool operator!=(const TaskSpecification& other) const { return !(*this == other); }
};

class TaskList {
private:
    int m_id;
    std::string m_name;
    std::string m_description;
    std::vector<Task> m_List;
    std::chrono::system_clock::time_point m_createDate;
    int m_userId;

    // Auxiliary Methods
    std::string toLower(std::string text);

public:
    // constructors
    TaskList() = delete;

    explicit TaskList(const int id, const std::string& name, const int userId)
        : m_id(id), m_name(name), m_userId(userId) {};

    explicit TaskList(const int id, const std::string& name, const std::string& description, const int userId)
        :  m_id(id), m_name(name), m_description(description), m_userId(userId) {};

    explicit TaskList(const int id, const std::string& name, const Task& task, const int userId);

    explicit TaskList(const int id, const std::string& name, const std::string& description, const Task& task, const int userId);

    // setters
    inline void setId (const int newId) { m_id = newId; };
    inline void setName(const std::string& newName) { m_name = newName; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }

    // getters
    [[nodiscard]] inline std::string getName() const { return m_name; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline size_t getSize() const { return m_List.size(); }
    [[nodiscard]] std::string getCreateDate() const;
    [[nodiscard]] std::optional<TaskSpecification> getTaskSpecification(const int id) const;

    // methods
    void addTask(const TaskSpecification& taskSpecification);
    void deleteTask(const int id);
    void editTask(const TaskSpecification& newTaskSpecification);
    void show() const;
    std::vector<TaskSpecification> search(const std::string& searchTerm);
};

#endif