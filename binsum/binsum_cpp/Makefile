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
	
all: binsum
	
%.o: %.cpp $(DEPS_HPP)
	$(CC) -g -fsanitize=fuzzer,undefined  -fprofile-instr-generate -fcoverage-mapping -fdeclspec -c $< $(CFLAGS)

fr_asm.o: fr.asm
	$(NASM) fr.asm -o fr_asm.o
	
binsum: $(DEPS_O) binsum.o
	# $(CC) -g -fsanitize=address,fuzzer,undefined  -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -o binsum *.o -lgmp ../../openssl-3.1.0/libssl.a ../../openssl-3.1.0/libcrypto.a -I../../openssl-3.1.0/include/ 
	$(CC) -g -fsanitize=fuzzer,address,undefined -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -lcrypto -lssl -o binsum *.o -lgmp

