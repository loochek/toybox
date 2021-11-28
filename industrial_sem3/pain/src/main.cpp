#include <cstdio>
#include <stdexcept>
#include "App.hpp"

#ifndef __has_feature
// GCC does not have __has_feature...
#define __has_feature(feature) 0
#endif

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#ifdef __cplusplus
extern "C"
#endif
const char *__asan_default_options() {
  // Clang reports ODR Violation errors in mbedtls/library/certs.c.
  // NEED TO REPORT THIS ISSUE
  return "detect_odr_violation=0";
}
#endif

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