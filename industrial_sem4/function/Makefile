build: function_tests

function_tests: tests.cpp function.hpp
	clang++ -std=c++20 -g -o function_tests -fsanitize=address tests.cpp

clean:
	rm -f function_tests
