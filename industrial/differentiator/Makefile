CC=clang++
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.cpp article.cpp diff.cpp simpler.cpp expr_tree.cpp parser.cpp node_pool.cpp file_utils.cpp lerror.cpp -o equator

clean:
	rm -f equator expr.dot expr.svg article.log article.aux article.pdf article.tex result.txt
