CC=clang

build:
	$(CC) main.c custom_strcmp.c -o onegin

build_debug:
	$(CC) -g main.c custom_strcmp.c -o onegin

run: build
	./onegin

test:
	$(CC) -DTEST custom_strcmp.c tests.c -o onegin
	./onegin

clean:
	rm -f onegin onegin_parsed.txt