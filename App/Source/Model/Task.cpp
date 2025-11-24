#include "Task.h"
#include "Utilities.h"

#include <string>

std::ostream& operator<<(std::ostream& output, const Task& task)
{
    output << "Task [" << task.getId() << "]: " << task.getTitle() << ", "
    << TaskUtilities::formatDate(task.getDueDate());
    return output;
}

std::ostream& operator<<(std::ostream& output, const TaskSpecification& specification)
{
    output << "Task [" << specification.Id << "]: " << specification.Title << ", "
    << TaskUtilities::formatDate(specification.DueDate) << ", " << specification.Status;
    return output;
}
