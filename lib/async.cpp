#include <set>
#include <mutex>
#include <memory>
#include <thread>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <condition_variable>
#include "async.h"
#include "commandparser.h"

namespace async {

//!
//!
//!
using Id = int;

//!
//! \brief packToString - упаковка в строку для печати
//! \param pack - блок команд
//! \param delim - разделитель
//! \return - строковое представление блока команд
//!
std::string packToString(const CommandsPack &pack, const std::string &delim)
{
    auto it = pack.cbegin();
    auto result = *it;
    for (++it;it != pack.cend();++it)
        result += (delim + *it);
    return result;
}

//!
//! \brief The Context class
//!
class Context
{
public:
    using Shared = std::shared_ptr<Context>;

    Context(const size_t &n);

    void processData(const char *dataBlock, const std::size_t &dataBlockSize)
    {
        for (std::size_t i = 0; i < dataBlockSize; ++i, ++dataBlock){
            if (*dataBlock != '\n' ) {
                m_buffer.append(std::string(dataBlock, 1));
            } else if (!m_buffer.empty()) {
                m_parser->pushLine(m_buffer);
                m_buffer.clear();
            }
        }
    }

    void processLast()
    {
        m_parser->handleData();
    }

private:
    std::shared_ptr<CommandParser> m_parser;
    std::string m_buffer;

};

//
// vars
//

// для работы со списком дескриптором контекста
std::mutex handleMutex;
std::set<Context::Shared> handlesList;

//
std::set<std::shared_ptr<std::thread>> threadsList;

// для вывода в cout
std::mutex receiveCoutMutex;
std::vector<CommandsPack> commandsCoutList;
std::condition_variable receiveCoutVar;

// для вывода в файл
std::mutex receiveFilesMutex;
std::vector<std::pair<std::time_t, CommandsPack>> commandsFilesList;
std::condition_variable receiveFilesVar;

// для создания дополнительных потоков
bool finish = true;
std::once_flag createExtraThreadFlag;

//!
//! \brief pushCommandsBlock
//! \param time
//! \param commands
//!
void pushCommandsBlock(const std::time_t &time, const CommandsPack &commands)
{
    {
        std::lock_guard<std::mutex> lock(receiveCoutMutex);
        commandsCoutList.push_back(commands);
    }
    receiveCoutVar.notify_all();

    {
        std::lock_guard<std::mutex> lock(receiveFilesMutex);
        commandsFilesList.push_back({time, commands});
    }
    receiveFilesVar.notify_one();
}

//!
//! \brief Context::Context
//! \param n
//!
Context::Context(const size_t &n) :
    m_parser(std::make_shared<CommandParser>(n, pushCommandsBlock))
{
    m_buffer.reserve(1024);
}

//!
//! \brief init - initialize library
//!
void init()
{
    std::call_once(createExtraThreadFlag, []()
    {
        finish = false;

        auto consoleLog = []()
        {
            std::vector<CommandsPack> data;
            while (!finish) {
                {
                    // ждём данных
                    std::unique_lock<std::mutex> lk(receiveCoutMutex);
                    receiveCoutVar.wait(lk, []{ return !commandsCoutList.empty(); });
                    if (finish)
                        break;
                    // забираем все данные
                    data.swap(commandsCoutList);
                }

                for (const auto &item: data) {
                    if (finish)
                        break;

                    // печать в консоль
                    std::cout << "bulk: " << packToString(item, ", ") << std::endl;
                }

                data.clear();
            }
            std::cout << "console log thread finished!" << std::endl;
        };

        auto filesLog = [](const Id &id)
        {
            std::vector<std::pair<std::time_t, CommandsPack>> data;
            while (!finish) {
                {
                    // ждём данных
                    std::unique_lock<std::mutex> lk(receiveFilesMutex);
                    receiveFilesVar.wait(lk, []{ return !commandsFilesList.empty(); });
                    if (finish)
                        break;
                    // забираем все данные
                    data.swap(commandsFilesList);
                }

                for (const auto &item: data) {
                    if (finish)
                        break;

                    // печать в файл
                    static const auto fmt = "./bulk%.10zu_%d.log";
                    static const int sz = std::snprintf(nullptr, 0, fmt, item.first, id);
                    std::string fileName(sz, '\0');
                    std::sprintf(fileName.data(), fmt, item.first, id);

                    std::ofstream file;
                    file.open(fileName);
                    const auto data = packToString(item.second, "\n");
                    file.write(data.c_str(), data.length());
                    file.close();
                }

                data.clear();
            }
            std::cout << "console log thread finished!" << std::endl;
        };

        // console log thread
        threadsList.emplace(new std::thread(consoleLog));
        threadsList.end()->get()->join();

        // two file log thread
        Id id = 0;
        threadsList.emplace(new std::thread(filesLog, ++id));
        threadsList.end()->get()->join();

        threadsList.emplace(new std::thread(filesLog, ++id));
        threadsList.end()->get()->join();
    });
}

//!
//! \brief deinit - deinit library
//!
void deinit()
{
    finish = true;
    pushCommandsBlock(std::time(nullptr), {});
}

//!
//! \brief connect - function of connction to ...
//! \param blockSize - count commands in static block
//! \return descriptor of connection
//!
Handle connect(const std::size_t &blockSize)
{
    std::lock_guard<std::mutex> lock(handleMutex);
    const auto handle = new Context(blockSize);
    handlesList.emplace(handle);
    return static_cast<Handle>(handle);
}

//!
//! \brief receive - receve block of data and process it
//! \param handle - connection descriptor
//! \param dataBlock
//! \param dataBlockSize
//!
void receive(const Handle &handle, const char *dataBlock, const std::size_t &dataBlockSize)
{
    const auto context = reinterpret_cast<Context *>(handle);
    if (context != nullptr)
        context->processData(dataBlock, dataBlockSize);
}

//!
//! \brief disconnect - close connection
//! \param handle - connection descriptor
//!
void disconnect(const Handle &handle)
{
    std::lock_guard<std::mutex> lock(handleMutex);
    const auto context = std::find_if(handlesList.cbegin(), handlesList.cend(), [handle](const Context::Shared &item)
    {
        return (item.get() == handle);
    });
    context->get()->processLast();
    handlesList.erase(context);
}

}

