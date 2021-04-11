make ARG=-DVALGRIND
valgrind --tool=callgrind --callgrind-out-file=callgrind.out.without_asm ./dict_test
make clean

make build_asm_hash ARG=-DVALGRIND
valgrind --tool=callgrind --callgrind-out-file=callgrind.out.asm_hash ./dict_test
make clean

make build_asm_both ARG=-DVALGRIND
valgrind --tool=callgrind --callgrind-out-file=callgrind.out.asm_both ./dict_test
make clean

make
time ./dict_test
make clean

make build_asm_hash
time ./dict_test
make clean

make build_asm_both
time ./dict_test
make clean