#include "Utils.h"
std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    return s.substr(first, s.find_last_not_of(" \t\r\n") - first + 1);
}
std::string cleanField(std::string s) {
    for (char& c : s) {
        if (c == '|' || c == '\n' || c == '\r')
            c = ' ';
    }

    return trim(s);
}
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return trim(value);
}
int readInt(const std::string& prompt, int low, int high) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
            return low;
        std::stringstream ss(line);
        int value;
        char extra;
        if ((ss >> value) && !(ss >> extra) &&
            value >= low && value <= high)
            return value;
        std::cout << "Enter a valid number from "
                  << low << " to " << high << ".\n";
    }
}
bool validTime(const std::string& s) {
    if (s.size() != 5 || s[2] != ':' ||
        !std::isdigit(static_cast<unsigned char>(s[0])) ||
        !std::isdigit(static_cast<unsigned char>(s[1])) ||
        !std::isdigit(static_cast<unsigned char>(s[3])) ||
        !std::isdigit(static_cast<unsigned char>(s[4])))
        return false;
    int h = std::stoi(s.substr(0, 2));
    int m = std::stoi(s.substr(3, 2));
    return h <= 23 && m <= 59;
}
bool validDate(const std::string& s) {
    if (s.size() != 10 || s[2] != '-' || s[5] != '-')
        return false;
    try {
        int d = std::stoi(s.substr(0, 2));
        int m = std::stoi(s.substr(3, 2));
        int y = std::stoi(s.substr(6, 4));
        if (y < 2000 || y > 2100 || m < 1 || m > 12)
            return false;
        const int days[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };
        int maxDay = days[m - 1];
        bool leap = y % 400 == 0 ||
                    (y % 4 == 0 && y % 100 != 0);
        if (m == 2 && leap)
            maxDay = 29;
        return d >= 1 && d <= maxDay;
    }
    catch (...) {
        return false;
    }
}
