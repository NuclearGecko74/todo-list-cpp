#include "Utilities.h"

namespace TaskUtilities
{
    // Returns date and time using ctime
    std::string formatDate(const std::chrono::system_clock::time_point& timePoint)
    {
        const std::time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);

        std::tm calendarDate{};
        std::stringstream formatter;
        if (localtime_s(&calendarDate, &rawTime) == 0)
        {
            formatter << std::put_time(&calendarDate, "%a %b %d %H:%M:%S %Y");
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
        std::chrono::system_clock::time_point timePoint;
        std::stringstream dateStream(dateString);

        dateStream >> std::chrono::parse("%F %T", timePoint);

        return timePoint;
    }
}
