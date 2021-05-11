test: fib fib.prg
	./test_native.sh
	./test_sw.sh

fib.prg:
	./tc_sw fib.tc -Ofib.prg

fib:
	./tc -o fib fib.tc

clean:
	rm -f fib
	rm -f fib.prg
