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
	
all: sha
	
%.o: %.cpp $(DEPS_HPP)
	$(CC) -g -fsanitize=address,fuzzer -fprofile-instr-generate -fcoverage-mapping -c $< $(CFLAGS)
	
# main.o: main.cpp $(DEPS_HPP)
# 	clang++ -g -fsanitize=address,fuzzer -o main.cpp

fr_asm.o: fr.asm
	$(NASM) fr.asm -o fr_asm.o
	
sha: $(DEPS_O) sha.o
	# $(CC) -g -fsanitize=address,fuzzer openssl-3.1.0/libssl.a openssl-3.1.0/libcrypto.a -std=c++17 -Iopenssl-3.1.0/include/ -lstdc++fs -ldl -lstdc++ -lssl -lcrypto -fprofile-instr-generate -fcoverage-mapping -c main.cpp $< $(CFLAGS)
	$(CC) -g -fsanitize=address,fuzzer -std=c++17 -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -o sha *.o -lgmp ../../openssl-3.1.0/libssl.a ../../openssl-3.1.0/libcrypto.a -I../../openssl-3.1.0/include/
        # $(CC) -g -fsanitize=address,fuzzer openssl-3.1.0/libssl.a openssl-3.1.0/libcrypto.a -std=c++17 -Iopenssl-3.1.0/include/ -lstdc++fs -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -c main.cpp $< $(CFLAGS)
        # $(CC) -g -fsanitize=address,fuzzer openssl-3.1.0/libssl.a openssl-3.1.0/libcrypto.a -std=c++17 -Iopenssl-3.1.0/include/ -lstdc++fs  -ldl -lstdc++ -fprofile-instr-generate -fcoverage-mapping -o sha *.o -lgmp 
        # $(CC) -g -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping -lcrypto -lssl -c main.cpp $< $(CFLAGS)
	# $(CC) -g -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping -lcrypto -lssl -o sha *.o -lgmp
	# $(CC) -o sha *.o -lgmp 
