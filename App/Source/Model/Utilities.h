//
// Created by frd on 19/11/2025.
//

#ifndef TODOLIST_UTILITIES_H
#define TODOLIST_UTILITIES_H

#include <chrono>
#include <string>

namespace TaskUtilities
{
    // Returns date and time using ctime
    std::string formatDate(const std::chrono::system_clock::time_point& time_point);

    std::string toLower(std::string text);
}

#endif //TODOLIST_UTILITIES_H