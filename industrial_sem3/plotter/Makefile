CC = clang++
CFLAGS = -fsanitize=address
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o plotter main.cpp SFMLApp.cpp Plot.cpp Vec2.cpp Mat3.cpp $(LDFLAGS)

clean:
	rm -f plotter
