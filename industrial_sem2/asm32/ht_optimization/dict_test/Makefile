CXX=clang++
CXXFLAGS=-O0 -DBENCHMARK -g
export CXX
export CXXFLAGS

build:
	$(MAKE) -C ../lstatus
	$(MAKE) -C ../dict
	$(MAKE) -C ../list
	$(CXX) $(CXXFLAGS) db.cpp dict_test.cpp ../lstatus/lstatus.o ../dict/dict.o ../list/list_common.o -o dict_test

clean:
	$(MAKE) -C ../lstatus clean
	$(MAKE) -C ../list clean
	$(MAKE) -C ../dict clean
	rm -f dict_test
