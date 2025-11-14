#ifndef TODOLIST_TASK_H
#define TODOLIST_TASK_H

#include <chrono>
#include <string>
#include <utility>

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
    explicit Task(const int taskId,std::string  title = "", const std::string&  description = "")
        : m_id(taskId), m_title(std::move(title)), m_description((description)),
        m_statusComplete(false), m_isDeleted(false)
    {
        m_createDate = std::chrono::system_clock::now();
    }

    // Setters
    inline void setId(const int newId) { m_id = newId; }
    inline void setTitle(const std::string& newTitle) { m_title = newTitle; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }
    inline void setStatus(const bool newStatus) { m_statusComplete = newStatus; }
    inline void setIsDeleted(const bool newIsDeleted) { m_isDeleted = newIsDeleted; }
    void setDueDate(std::tm timeStruct = std::tm());

    // Getters
    [[nodiscard]] inline int getId() const { return m_id; }
    [[nodiscard]] inline std::string getTitle() const { return m_title; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline bool getStatus() const { return m_statusComplete; } // false = in progress, true = completed
    [[nodiscard]] inline bool getStatusDelete() const { return m_isDeleted; }
    [[nodiscard]] inline std::chrono::system_clock::time_point getIsDeleted() const { return m_createDate; }
    [[nodiscard]] std::string getDueDate() const;
};

// Operators
std::ostream& operator<<(std::ostream& output, const Task& task);


#endif