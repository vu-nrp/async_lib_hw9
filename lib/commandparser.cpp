#include <iostream>
#include "commandparser.h"

const std::string CommandParser::DynBlockBeg = "{";
const std::string CommandParser::DynBlockEnd = "}";

//!
//! \brief CommandParser::CommandParser - констрктор класса
//! \param n - размер статического блока команд
//!
CommandParser::CommandParser(const size_t &n, Notifier f) :
    m_packLen(n),
    m_notifier(f)
{
    m_commands.reserve(m_packLen);
}

//!
//! \brief CommandParser::pushLine - получение и анализ пришедшего ввода
//! \param line - строка ввода
//!
void CommandParser::pushLine(const std::string &line)
{
    if (line == CommandParser::DynBlockBeg) {
        // вложенный динамический блок { (начало) - пропускаем '{' и продолжаем накапливать команды
        if (m_isDynActivated != 0) {
            m_isDynActivated++;
            return;
        }

        // старт динамического блока, печатаем уже накопленное и начинаем накапливать с начала
        handleData();

        m_isDynActivated++;
    } else if (line == CommandParser::DynBlockEnd) {
        if (m_isDynActivated != 0) {
            m_isDynActivated--;

            if (m_isDynActivated != 0) {
                // вложенный динамический блок } (конец) - пропускаем '}' и продолжаем накапливать команды
                return;
            }

            // печатаем уже накопленное и начинаем накапливать с начала
            handleData();
        } else {
            std::cout << "unexpected dynamic block ending '}' - skip" << std::endl;
            return;
        }
    } else {
        if (m_commands.empty()) {
            // запоминаем время прихода 1й команды из блока
            m_packStartTime = std::time(nullptr);
        }

        // сохраняем команду в блок
        m_commands.push_back(line);

        // накопили N команд, печатаем и начинаем накапливать с начала
        if (m_commands.size() == m_packLen) {
            // если достаточно команд и это не динамический блок
            handleData();
        }
    }
}

//!
//! \brief CommandParser::handleData - обработка данных в случае соблюдения условий
//!
void CommandParser::handleData()
{
    if ((m_isDynActivated == 0) && !m_commands.empty()) {
        notifyAndClear();
    }
}

void CommandParser::notifyAndClear()
{
    // извещаем подписчиков
    m_notifier(m_packStartTime, m_commands);
    // очищаем буфер
    m_commands.clear();
}
