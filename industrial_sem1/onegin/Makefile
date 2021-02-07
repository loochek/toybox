CC=clang
SRC_FILES= main.c string_lib.c sort.c tests.c lerror.c
CFLAGS=-std=c99

build:
	$(CC) $(CFLAGS) $(SRC_FILES) -o onegin

build_debug:
	$(CC) -g $(CFLAGS) $(SRC_FILES) -o onegin -DTEST

run: build
	./onegin onegin.txt onegin_output.txt

test:
	$(CC) -DTEST $(CFLAGS) $(SRC_FILES) -o onegin -fsanitize=address
	./onegin

clean:
	rm -f onegin onegin_output.txt