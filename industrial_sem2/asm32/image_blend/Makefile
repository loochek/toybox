C=clang
CFLAGS=-mavx2 -lcsfml-window -lcsfml-graphics -lcsfml-system -O3

build:
	$(C) $(CFLAGS) main.c sfml_app.c image_blend.c -o mandelbrot

clean:
	rm -f mandelbrot