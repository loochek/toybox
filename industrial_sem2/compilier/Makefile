CXX=clang++
CXXFLAGS=-g

build:
	$(CXX) $(CXXFLAGS) -o tc \
		main.cpp \
		lexer.cpp \
		parser.cpp \
		code_gen.cpp \
		binary_emitter.cpp \
		ast.cpp \
		compilation_error.cpp \
		utils/file_utils.cpp \
		utils/list_common.cpp \
		utils/lstatus.cpp \
		utils/string_view.cpp

clean:
	rm -f tc *.dot *.svg