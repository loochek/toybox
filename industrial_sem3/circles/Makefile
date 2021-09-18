CC = clang++
CFLAGS = -O3
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o raycaster main.cpp App.cpp SFMLGraphics.cpp Circle.cpp PhysicalWorld.cpp PhysicalCircle.cpp $(LDFLAGS)

clean:
	rm -f raycaster
