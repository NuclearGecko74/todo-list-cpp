#ifndef TODOLIST_TASK_H
#define TODOLIST_TASK_H

#include <chrono>
#include <string>
#include <utility>

class Task {
private:
    std::string m_title;
    std::string m_description;
    bool m_statusComplete;
    std::chrono::system_clock::time_point m_dueDate; // Date & time

public:
    explicit Task(std::string  title = "", std::string  description = "")
        : m_title(std::move(title)), m_description(std::move(description)), m_statusComplete(false) {}

    // Setters
    inline void setTitle(const std::string& newTitle) { m_title = newTitle; }
    inline void setDescription(const std::string& newDescription) { m_description = newDescription; }
    inline void setStatus(bool newStatus) { m_statusComplete = newStatus; }
    void setDueDate(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

    // Getters
    [[nodiscard]] inline std::string getTitle() const { return m_title; }
    [[nodiscard]] inline std::string getDescription() const { return m_description; }
    [[nodiscard]] inline bool getStatus() const { return m_statusComplete; } // false = in progress, true = completed
    [[nodiscard]] std::string getDueDate() const;
};
#endif //TODOLIST_TASK_H