#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
std::string trim(const std::string& s);
std::string cleanField(std::string s);
std::string readLine(const std::string& prompt);
int readInt(const std::string& prompt, int low, int high);
bool validTime(const std::string& value);
bool validDate(const std::string& value);
#endif
