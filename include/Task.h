#ifndef TODOLIST_TASK_H
#define TODOLIST_TASK_H
#include <chrono>
#include <string>


class Task {
private:
    std::string title;
    std::string description;
    bool statusComplete;
    std::chrono::system_clock::time_point dueDate; // Fecha y hora

public:
    Task(const std::string& tit = "", const std::string& desc = "")
        : title(tit), description(desc), statusComplete(false) {}

    void setTitle(std::string newTitle);

    std::string getTitle();

    void setDescription(std::string newDescription);

    std::string getDescription();

    void setStatus(bool newStatus);

    //Retorna el estado en forma de bool (false = en progreso, true = completada)
    bool getStatus();

    void setDueDate(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

    // Retorna la fecha como string desde un ctime_s usando un bufer
    std::string getDueDate() const;
};
#endif //TODOLIST_TASK_H