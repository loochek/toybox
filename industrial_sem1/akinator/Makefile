CC=clang
CFLAGS=-fsanitize=address -g

build:
	$(CC) $(CFLAGS) main.c akinator.c search_tree.c file_utils.c allocator.c lerror.c stack/stack_impl.c -o akinator

clean:
	rm -f akinator speech.wav tree.dot tree.svg
	cp AkinatorDB/akinator.db akinator.db
