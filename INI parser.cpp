#include <iostream>
#include "INI parser.h"
#include <fstream>
#include <clocale>
#include <Windows.h>

ini_parser::ini_parser(const std::string& filename)
{
    load(filename);
}

void ini_parser::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удается открыть файл: " + filename);
    }

    std::string line;
    std::size_t str = 0;

    while (std::getline(file, line))
    {
        ++str;
        process_line(line, str);
    }

    if (!file.eof() && file.fail())
    {
        throw std::runtime_error("Ошибка при чтении файла: " + filename);
    }
}

void ini_parser::process_line(const std::string& raw, std::size_t str)
{
    const std::string line = trim(raw);

    if (line.empty() || line.front() == ';')
    {
        return;
    }

    if (line.front() == '[')
    {
        if (line.back() != ']')
        {
            throw std::runtime_error(
                "Строка " + std::to_string(str) + ": неправильно сформированный заголовок раздела " + raw + "\"");
        }
        current_section_ = trim(line.substr(1, line.size() - 2));

        if (current_section_.empty())
        {
            throw std::runtime_error(
                "Строка " + std::to_string(str) + ": пустое имя раздела");
        }
        data_[current_section_];
        return;
    }

    if (current_section_.empty())
    {
        throw std::runtime_error(
            "Строка " + std::to_string(str) + ": переменная вне раздела");
    }

    const std::size_t eq = line.find('=');
    if (eq == std::string::npos)
    {
        throw std::runtime_error(
            "Строка " + std::to_string(str) + ": ожидалось name = value, получено " + raw);
    }

    const std::string name = trim(line.substr(0, eq));
    std::string value = trim(line.substr(eq + 1));

    const std::size_t comment = value.find(';');
    if (comment != std::string::npos)
    {
        value = trim(value.substr(0, comment));
    }

    if (name.empty())
    {
        throw std::runtime_error(
            "Строка " + std::to_string(str) + ": пустое имя");
    }
    data_[current_section_][name] = value;
}

std::string ini_parser::trim(const std::string& s)
{
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}


int main()
{
    setlocale(LC_CTYPE, "ru_RU.UTF-8");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    const std::string filename = "config.ini";
    try
    {
        ini_parser parser(filename);

        auto var1 = parser.get_value<double>("Section1.var1");
        auto var2 = parser.get_value<std::string>("Section1.var2");
        auto var3 = parser.get_value<std::string>("Section1.var3");
        auto mode = parser.get_value<std::string>("Section4.Mode");
        auto Vid = parser.get_value<std::string>("Section4.Vid");

        std::cout << "Section1.var1 = " << var1 << "\n"
                  << "Section1.var2 = " << var2 << "\n"
                  << "Section1.var3 = " << var3 << "\n"
                  << "Section4.Mode = " << mode << "\n"
                  << "Section4.Vid  = " << Vid << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
