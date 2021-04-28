CXX=clang++
CXXFLAGS=-g -fsanitize=address

build:
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

run: build
	./tc
	cat prologue.asm output.asm > prg.asm
	nasm -f elf64 prg.asm
	clang -c stdlib.c
	clang -o prg stdlib.o prg.o

clean:
	rm -f tc *.dot *.svg *.o prg.asm output.asm prg