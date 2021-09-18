CC = clang++
CFLAGS = -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o raycaster main.cpp App.cpp SFMLGraphics.cpp Circle.cpp PhysicalWorld.cpp $(LDFLAGS)

clean:
	rm -f raycaster
