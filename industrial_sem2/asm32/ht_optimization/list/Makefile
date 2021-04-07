build: list_common.o

list_test: build_list_test
	./list_test

build_list_test: tests.o
	$(MAKE) -C ../lstatus
	$(CXX) tests.o ../lstatus/lstatus.o -o list_test

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f list_test *.o