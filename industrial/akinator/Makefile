CC=clang
CFLAGS=-fsanitize=address -g

build:
	$(CC) $(CFLAGS) main.c akinator.c search_tree.c file_utils.c lerror.c -o akinator

clean:
	rm -f akinator