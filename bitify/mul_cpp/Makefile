CC=clang++
CFLAGS=-std=c++11 -O3 -I.
DEPS_HPP = circom.hpp calcwit.hpp fr.hpp
DEPS_O = main.o calcwit.o fr.o fr_asm.o

ifeq ($(shell uname),Darwin)
	NASM=nasm -fmacho64 --prefix _
endif
ifeq ($(shell uname),Linux)
	NASM=nasm -felf64
endif
	
all: mul
	
%.o: %.cpp $(DEPS_HPP)
	$(CC) -g -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping -c $< $(CFLAGS)
	
# main.o: main.cpp $(DEPS_HPP)
# 	clang++ -g -fsanitize=address,fuzzer -o main.cpp

fr_asm.o: fr.asm
	$(NASM) fr.asm -o fr_asm.o
	
mul: $(DEPS_O) mul.o
	$(CC) -g -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping -c main.cpp $< $(CFLAGS)
	$(CC) -g -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping -o mul *.o -lgmp 
	# $(CC) -o mul *.o -lgmp 