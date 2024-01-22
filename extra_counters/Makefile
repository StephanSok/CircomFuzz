CC=clang++
CFLAGS=-std=c++11 -O3 -I.
# DEPS_HPP = circom.hpp calcwit.hpp fr.hpp
DEPS_O = main.o

ifeq ($(shell uname),Darwin)
	NASM=nasm -fmacho64 --prefix _
endif
ifeq ($(shell uname),Linux)
	NASM=nasm -felf64
endif
	
all: test
	
%.o: %.cpp $(DEPS_HPP)
	$(CC) -g -fsanitize=fuzzer,undefined  -fprofile-instr-generate -fcoverage-mapping -fdeclspec -c $< $(CFLAGS)

fr_asm.o: fr.asm
	$(NASM) fr.asm -o fr_asm.o
	
test: $(DEPS_O) main.o
	$(CC) -g -fsanitize=address,fuzzer,undefined  -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -o test *.o -lgmp 
