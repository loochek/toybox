CC = clang++
CFLAGS = -O3
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o circles main.cpp App.cpp SFMLGraphics.cpp Circle.cpp Square.cpp PhysicalWorld.cpp PhysicalCircle.cpp $(LDFLAGS)

clean:
	rm -f circles
