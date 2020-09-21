CC=clang

build:
	$(CC) main.c file_utils.c sort.c tests.c -o onegin

build_debug:
	$(CC) -g main.c file_utils.c sort.c tests.c -o onegin

run: build
	./onegin

test:
	$(CC) -DTEST main.c file_utils.c sort.c tests.c -o onegin
	./onegin

clean:
	rm -f onegin onegin_parsed.txt