#ifndef TODOLIST_TASKLIST_H
#define TODOLIST_TASKLIST_H

#include <vector>
#include "Task.h"
#include "TaskManager.h"

struct TaskListSpecification
{
    int Id = -1;
    std::string Name = "Untitled List";
    std::string Description;
    bool IsDeleted = false;
    std::chrono::system_clock::time_point CreateDate = std::chrono::system_clock::now();
    int UserId = -1;
};

class TaskList {
private:
    int m_id;
    std::string m_name;
    std::string m_description;
    std::vector<Task> m_list;
    bool m_isDeleted;
    std::chrono::system_clock::time_point m_createDate;
    int m_userId;
    TaskManager& m_taskManager;

public:
    // constructors
    explicit TaskList(const TaskListSpecification& specification, TaskManager& taskManager)
        : m_id(specification.Id), m_name(specification.Name), m_description(specification.Description),
        m_isDeleted(specification.IsDeleted), m_createDate(specification.CreateDate),
        m_userId(specification.UserId), m_taskManager(taskManager)
    {
        m_list = m_taskManager.loadTasks(m_id);
    }

    // setters
    inline void setId (const int newId) { m_id = newId; };
    inline void setName(const std::string& newName) { m_name = newName; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }
    inline void setIsDeleted(const bool newIsDeleted) { m_isDeleted = newIsDeleted; }
    inline void setUserId(const int newUserId) { m_userId = newUserId; }

    // getters
    [[nodiscard]] inline int getId() const { return m_id; }
    [[nodiscard]] inline std::string getName() const { return m_name; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline bool getIsDeleted() const { return m_isDeleted; }
    [[nodiscard]] inline size_t getSize() const { return m_list.size(); }
    [[nodiscard]] inline std::chrono::system_clock::time_point getCreateDate() const { return m_createDate; }
    [[nodiscard]] inline int getUserId() const { return m_userId; }
    [[nodiscard]] std::optional<TaskSpecification> getTaskSpecification(const int id) const;

    // methods
    void addTask(const TaskSpecification& taskSpecification);
    void deleteTask(const int id);
    void editTask(const TaskSpecification& newTaskSpecification);
    void show() const;
    [[nodiscard]] std::vector<TaskSpecification> search(const std::string& searchTerm) const;
    void deleteList();
};

#endif