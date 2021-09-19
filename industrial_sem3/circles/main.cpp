#include <cstdio>
#include <stdexcept>
#include "App.hpp"

int main()
{
    try
    {
        App app;
        app.run();
    }
    catch (const std::exception& e)
    {
        printf("Error: %s\n", e.what());
    }
    
    return 0;
}