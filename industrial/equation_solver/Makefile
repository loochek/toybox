CC=clang
LIBS=-lm

build:
	$(CC) $(LIBS) main.c solver.c -o equation_solver

run: build
	./equation_solver

test:
	$(CC) $(LIBS) -DTEST tests.c solver.c -o equation_solver
	./equation_solver

clean:
	rm equation_solver
