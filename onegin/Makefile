CC=clang

build:
	$(CC) main.c custom_strcmp.c tests.c -o onegin

run: build
	./onegin

test:
	$(CC) -DTEST main.c custom_strcmp.c tests.c -o onegin
	./onegin

clean:
	rm -f onegin onegin_parsed.txt