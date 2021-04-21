CXX=clang++
CXXFLAGS=-g -DDEBUG -fsanitize=address

build:
	$(CXX) $(CXXFLAGS) -o tc main.cpp lexer.cpp parser.cpp ast.cpp compilation_error.cpp utils/file_utils.cpp utils/list_common.cpp utils/lstatus.cpp utils/string_view.cpp

clean:
	rm -f tc *.dot *.svg