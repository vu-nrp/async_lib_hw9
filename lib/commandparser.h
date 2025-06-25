#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <functional>

//!
//! CommandsPack - block of command
//!
using CommandsPack = std::vector<std::string>;


//!
//! \brief The CommandParser class - класс для анализа ввода и извещения подписчиков
//!
class CommandParser// : public Publisher
{
public:
    using Notifier = std::function<void(const std::time_t &time, const CommandsPack &commands)>;

    static const std::string DynBlockBeg;
    static const std::string DynBlockEnd;

    CommandParser(const size_t &n, Notifier f);

    void pushLine(const std::string &line);
    void handleData();
    void notifyAndClear();

private:
    size_t m_packLen {0};
    Notifier m_notifier {nullptr};
    CommandsPack m_commands;
    std::time_t m_packStartTime {0};
    size_t m_isDynActivated {0};

};
