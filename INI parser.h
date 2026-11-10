#pragma once
#include <cstddef>
#include <map>
#include <sstream> 
#include <stdexcept>
#include <string>
#include <type_traits> 

class ini_parser
{
public:
    explicit ini_parser(const std::string& filename);

    template <typename T>
    T get_value(const std::string& key) const
    {
        const std::size_t dot = key.find('.');
        if (dot == std::string::npos || dot == 0)
        {
            throw std::invalid_argument("Неверный формат ключа " + key);
        }

        const std::string section = key.substr(0, dot);
        const std::string variable = key.substr(dot + 1);

        const auto sec_it = data_.find(section);
        if (sec_it == data_.end())
        {
            throw std::runtime_error("Раздел " + section + " не найден");
        }

        const auto var_it = sec_it->second.find(variable);
        if (var_it == sec_it->second.end())
        {
            throw std::runtime_error("Переменная " + variable + " не найдено в разделе " + section);
        }
        return convert<T>(var_it->second, key);
    }

private:
    std::map<std::string, std::map<std::string, std::string>> data_;
    std::string current_section_;

    void load(const std::string& filename);

    void process_line(const std::string& raw, std::size_t line_no);

    static std::string trim(const std::string& s);

    template <typename T>
    static T convert(const std::string& value, const std::string& key)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return value;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            std::istringstream stream(value);
            long long result = 0;
            stream >> result;

            if (stream.fail() || !stream.eof())
            {
                throw std::runtime_error("Переменная " + key + " не удается преобразовать " + value + " в целое число");
            }
            return static_cast<T>(result);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::istringstream stream(value);
            double result = 0.0;
            stream >> result;
            if (stream.fail() || !stream.eof())
            {
                throw std::runtime_error("Переменная " + key + " невозможно преобразовать " + value);
            }
            return static_cast<T>(result);
        }
        else
        {
            static_assert(std::is_same_v<T, void>,"get_value поддерживает int, double и string");
        }
    }
};
