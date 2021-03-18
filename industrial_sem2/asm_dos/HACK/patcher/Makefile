CC=clang
CFLAGS=-lcdk -lncurses -fsanitize=address

build:
	$(CC) $(CFLAGS) -o patcher patcher.c lerror.c

clean:
	rm -f patcher