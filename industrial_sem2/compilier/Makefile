CC=clang
CXX=clang++
CXXFLAGS=-g -DMEMORY_POOL_DEBUG -fsanitize=address

build: build_stdlib
	$(CXX) $(CXXFLAGS) -o tc \
		main.cpp \
		lexer.cpp \
		parser.cpp \
		code_gen.cpp \
		var_table.cpp \
		binary_emitter.cpp \
		ast.cpp \
		compilation_error.cpp \
		utils/file_utils.cpp \
		utils/list_common.cpp \
		utils/lstatus.cpp \
		utils/string_view.cpp

build_stdlib:
	cd stdlib; \
	$(CC) -O3 -c -o stdlib.o stdlib.c; \
	./header_gen.py

clean:
	rm -f tc *.dot *.svg *.o *.lst a.out stdlib/*.o stdlib/stdlib_blob.hpp