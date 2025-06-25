#include <string>
#include <vector>
#include <iostream>
#include "async.h"

//!
//! \brief main - program main function
//!
int main(int argc, char **argv)
{
#ifdef DEBUG_ON
    std::cout << "Home work #9" << std::endl;
#endif

    if (argc != 2) {
        std::cout << "one param required, it is a count of commands in block" << std::endl;
        return 1;
    }

    const std::string param(argv[1]);
    try {
        const size_t N = std::stol(param);

        if (N == 0) {
            std::cout << "invalid block size, block size must be more then 0" << std::endl;
            return 2;
        }

#ifdef DEBUG_ON
        // test data
        std::vector<std::string> testData {
            "command01",
            "command02",
            "command03",
            "command04",
            "command05",
            "{",
            "command06",
            "command07",
            "command08",
            "command09",
            "command10",
            "command11",
            "}",
            "command12",
            "command13",
            "command14",
            "command15",
            "{",
            "command16",
            "command17",
            "{",
            "command18",
            "command19",
            "command20",
            "}",
            "command21",
            "command22",
            "}",
            "command23",
            "command24",
            "command25",
            "{",
            "command26"
        };
#endif
        //
        async::init();

        // begin handling
        const auto handle = async::connect(N);
#ifdef DEBUG_ON
        for (auto line: testData) {
#else
        for (std::string line; std::getline(std::cin, line);) {
#endif
            line += "\n";
            async::receive(handle, line.c_str(), line.length());
        }
        async::disconnect(handle);

    } catch (std::invalid_argument const &ex) {
        std::cout << "invalid parameter: " << param << std::endl;
        return 3;
    } catch (std::out_of_range const &ex) {
        std::cout << "invalid parameter: " << param << std::endl;
        return 4;
    }

    //
    async::deinit();

    return 0;
}
