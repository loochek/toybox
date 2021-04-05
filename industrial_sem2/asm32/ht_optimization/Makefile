CXX=clang++
CXXFLAGS=-O0 -g
export CXX
export CXXFLAGS

build:
	$(MAKE) -C lstatus
	$(MAKE) -C dict
	$(MAKE) -C list
	$(CXX) main.cpp lstatus/lstatus.o dict/dict.o -o dict_test

clean:
	$(MAKE) -C lstatus clean
	$(MAKE) -C list clean
	rm -f dict_test