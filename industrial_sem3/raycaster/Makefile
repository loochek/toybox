CC = clang++
CFLAGS = -O3
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lm

build:
	$(CC) $(CFLAGS) -o raycaster main.cpp SFMLApp.cpp Raycaster.cpp 3DUtils.cpp $(LDFLAGS)

clean:
	rm -f raycaster
