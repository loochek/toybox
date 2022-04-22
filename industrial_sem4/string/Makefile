build: string_test

string_test: test.cpp String.hpp StringBase.hpp StringView.hpp
	clang++ -std=c++20 -g -o string_test -fsanitize=address test.cpp

clean:
	rm -f string_test
