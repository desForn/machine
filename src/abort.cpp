#include "fwd.hpp"

namespace Machine
{
    [[noreturn]] void abort(std::string s)
    {
        std::cerr << "In Machine::" << std::move(s) << ":\nUnexpected error.\nAborting...\n";
        std::cerr << std::flush;
        std::abort();
    }
}
