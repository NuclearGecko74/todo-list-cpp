#ifndef TODOLIST_TASK_H
#define TODOLIST_TASK_H

#include <chrono>
#include <string>

struct TaskSpecification {
    int Id = -1;
    std::string Title = "Unnamed Task";
    std::string Description;
    bool Status = false;
    bool isDeleted = false;
    std::chrono::system_clock::time_point DueDate;
    std::chrono::system_clock::time_point CreateDate = std::chrono::system_clock::now();

    // Operators
    bool operator==(const TaskSpecification& other) const { return Id == other.Id; }
    bool operator!=(const TaskSpecification& other) const { return !(*this == other); }
    friend std::ostream& operator<<(std::ostream& output, const TaskSpecification& specification);
};

class Task {
private:
    int m_id;
    std::string m_title;
    std::string m_description;
    bool m_statusComplete;
    bool m_isDeleted;
    std::chrono::system_clock::time_point m_createDate;
    std::chrono::system_clock::time_point m_dueDate; // Date & time

public:
    explicit Task(const TaskSpecification& specification)
        : m_id(specification.Id), m_title(specification.Title), m_description(specification.Description),
        m_statusComplete(specification.Status), m_isDeleted(specification.isDeleted),
        m_createDate(specification.CreateDate), m_dueDate(specification.DueDate) {}

    // Setters
    inline void setId(const int newId) { m_id = newId; }
    inline void setTitle(const std::string& newTitle) { m_title = newTitle; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }
    inline void setStatus(const bool newStatus) { m_statusComplete = newStatus; }
    inline void setIsDeleted(const bool newIsDeleted) { m_isDeleted = newIsDeleted; }
    inline void setDueDate(std::chrono::system_clock::time_point newDueDate) { m_dueDate = newDueDate; }

    // Getters
    [[nodiscard]] inline int getId() const { return m_id; }
    [[nodiscard]] inline std::string getTitle() const { return m_title; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline bool getStatus() const { return m_statusComplete; } // false = in progress, true = completed
    [[nodiscard]] inline bool getIsDeleted() const { return m_isDeleted; }
    [[nodiscard]] inline std::chrono::system_clock::time_point getCreateDate() const { return m_createDate; }
    [[nodiscard]] inline std::chrono::system_clock::time_point getDueDate() const { return m_dueDate; }

    // Operators
    friend std::ostream& operator<<(std::ostream& output, const Task& task);
};

#endif