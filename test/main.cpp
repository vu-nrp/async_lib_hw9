#include <iostream>
#include "async.h"

//!
//! \brief main
//!

int main(int argc, char **argv)
{
    std::cout << "Home work #9" << std::endl;

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

        //
        async::init();

        // begin handling
        const auto handle = async::connect(N);
        for (std::string line; std::getline(std::cin, line);) {
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
