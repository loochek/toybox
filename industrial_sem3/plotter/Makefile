CC = clang
CFLAGS = -g
LDFLAGS = -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

build:
	$(CC) $(CFLAGS) -o plotter main.c sfml_app.c plot.c lstatus.c $(LDFLAGS)

clean:
	rm -f plotter