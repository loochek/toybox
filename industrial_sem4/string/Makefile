build: string_tests

string_tests: tests.cpp String.hpp StringBase.hpp StringView.hpp
	clang++ -std=c++20 -g -o string_tests -fsanitize=address tests.cpp

clean:
	rm -f string_tests
