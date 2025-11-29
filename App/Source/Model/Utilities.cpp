#include "Utilities.h"

#include <chrono>
#include <format>
#include <sstream>

namespace TaskUtilities
{
    // Ensures the correct localtime function is called regardless of the OS
    bool safe_localtime(const std::time_t* time, std::tm* tm_out)
    {
        #if defined(_WIN32) || defined(_MSC_VER)
            return localtime_s(tm_out, time) == 0;
        #else
            return localtime_r(time, tm_out) != nullptr;
        #endif
    }

    std::string formatDate(const std::chrono::system_clock::time_point& timePoint)
    {
        const std::time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);
        std::tm calendarDate{};
        std::stringstream formatter;

        if (safe_localtime(&rawTime, &calendarDate))
        {
            formatter << std::put_time(&calendarDate, "%a %b %d %H:%M:%S %Y");
        }
        return formatter.str();
    }

    std::string timePointToString(const std::chrono::system_clock::time_point &timePoint)
    {
        const std::time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);
        std::tm calendarDate{};
        std::stringstream formatter;

        if (safe_localtime(&rawTime, &calendarDate))
        {
            // SQL date format
            formatter << std::put_time(&calendarDate, "%Y-%m-%d %H:%M:%S");
        }
        return formatter.str();
    }

    std::string toLower(std::string text)
    {
        for (auto& character : text)
        {
            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        }

        return text;
    }

    std::chrono::system_clock::time_point stringToTimePoint(const std::string &dateString)
    {
        std::tm tm = {};
        std::stringstream dateStream(dateString);

        dateStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (dateStream.fail())
        {
            return std::chrono::system_clock::from_time_t(0);
        }

        std::time_t time = std::mktime(&tm);

        if (time == -1)
        {
            return std::chrono::system_clock::from_time_t(0);
        }

        return std::chrono::system_clock::from_time_t(time);
    }
}