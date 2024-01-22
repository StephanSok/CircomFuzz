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
	
all: mont
	
%.o: %.cpp $(DEPS_HPP)
	$(CC) -g -fsanitize=fuzzer -fprofile-instr-generate -fcoverage-mapping -c $< $(CFLAGS)

fr_asm.o: fr.asm
	$(NASM) fr.asm -o fr_asm.o
	
mont: $(DEPS_O) mont.o
	$(CC) -g -fsanitize=address,fuzzer -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -o mont *.o -lgmp ../../openssl-3.1.0/libssl.a ../../openssl-3.1.0/libcrypto.a -I../../openssl-3.1.0/include/
	# $(CC) -g -fsanitize=fuzzer,address -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -lcrypto -lssl -o mont *.o -lgmp
