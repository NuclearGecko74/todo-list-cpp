#include "Task.h"
#include "Utilities.h"

#include <string>

std::ostream& operator<<(std::ostream& output, const Task& task)
{
    output << "Task [" << task.getId() << "]: " << task.getTitle() << ", "
    << TaskUtilities::formatDate(task.getDueDate());
    return output;
}