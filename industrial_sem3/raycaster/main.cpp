#include <cstdio>
#include "SFMLApp.hpp"

int main()
{
    try
    {
        SFMLApp app;
        app.run();
    }
    catch (const std::exception& e)
    {
        printf("Error: %s\n", e.what());
    }
    
    return 0;
}