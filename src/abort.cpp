#include "fwd.hpp"

#include <iostream>

namespace Machine
{
    [[noreturn]] void abort(const std::string &s)
    {
        std::cerr << "In Machine::" << s << ":\nUnexpected error.\nAborting...\n" << std::flush;
        std::abort();
    }
}
