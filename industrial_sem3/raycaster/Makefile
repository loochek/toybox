CC = clang++
CFLAGS = -fsanitize=address
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o raycaster main.cpp SFMLApp.cpp Raycaster.cpp MathUtils.cpp $(LDFLAGS)

clean:
	rm -f raycaster
