#ifndef TODOLIST_TASKLIST_H
#define TODOLIST_TASKLIST_H

#include <vector>
#include "Task.h"

struct TaskListSpecification
{
    int Id = -1;
    std::string Name = "Untitled List";
    std::string Description;
    std::chrono::system_clock::time_point CreateDate = std::chrono::system_clock::now();
    int UserId = -1;
};

class TaskList {
private:
    int m_id;
    std::string m_name;
    std::string m_description;
    std::vector<Task> m_List;
    std::chrono::system_clock::time_point m_createDate;
    int m_userId;

public:
    // constructors
    explicit TaskList(const TaskListSpecification& specification)
        : m_id(specification.Id), m_name(specification.Name), m_description(specification.Description),
        m_createDate(specification.CreateDate), m_userId(specification.UserId) {}

    explicit TaskList(const TaskListSpecification& specification, const Task& task)
        : m_id(specification.Id), m_name(specification.Name), m_description(specification.Description),
        m_createDate(specification.CreateDate), m_userId(specification.UserId)
    {
        m_List.emplace_back(task);
    }

    // setters
    inline void setId (const int newId) { m_id = newId; };
    inline void setName(const std::string& newName) { m_name = newName; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }
    inline void setUserId(const int newUserId) { m_userId = newUserId; }

    // getters
    [[nodiscard]] inline int getId() const { return m_id; }
    [[nodiscard]] inline std::string getName() const { return m_name; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline size_t getSize() const { return m_List.size(); }
    [[nodiscard]] inline std::chrono::system_clock::time_point getCreateDate() const { return m_createDate; }
    [[nodiscard]] inline int getUserId() const { return m_userId; }
    [[nodiscard]] std::optional<TaskSpecification> getTaskSpecification(const int id) const;

    // methods
    void addTask(const TaskSpecification& taskSpecification);
    void deleteTask(const int id);
    void editTask(const TaskSpecification& newTaskSpecification);
    void show() const;
    std::vector<TaskSpecification> search(const std::string& searchTerm) const;
};

#endif