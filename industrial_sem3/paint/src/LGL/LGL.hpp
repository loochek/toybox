#ifndef LGL_HPP
#define LGL_HPP

#ifdef LGL_USE_SDL
#error Unfortunately, SDL backend has not been implemented yet
#else
#include "SFML/SFMLRenderWindow.hpp"
#include "SFML/SFMLRenderTexture.hpp"
#endif

#endif