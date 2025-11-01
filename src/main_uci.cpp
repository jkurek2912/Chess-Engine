#include "engine/UCI.h"
#include <iostream>

int main()
{
    try
    {
        UCI::run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error\n";
        return 1;
    }
    return 0;
}

