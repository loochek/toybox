CC = clang++
CFLAGS = -O3
LDFLAGS_SFML = -lsfml-graphics -lsfml-window -lsfml-system -lm
LDFLAGS_SDL  = -lSDL2main -lSDL2 -lSDL2_gfx -lm

COMMON_SOURCES = main.cpp App.cpp DrawableCircleComponent.cpp DrawableSquareComponent.cpp PhysicalWorld.cpp PhysicalObject.cpp

build_sfml:
	$(CC) $(CFLAGS) -o circles $(COMMON_SOURCES) SFMLGraphics.cpp $(LDFLAGS_SFML)

build_sdl:
	$(CC) $(CFLAGS) -o circles -DCIRCLES_USE_SDL $(COMMON_SOURCES) SDLGraphics.cpp $(LDFLAGS_SDL)

clean:
	rm -f circles
