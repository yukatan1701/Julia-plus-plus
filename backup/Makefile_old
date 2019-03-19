CC=g++

CFLAGS=-std=c++11 -fmax-errors=5

all: hello

hello:
	$(CC) main.cpp lexical_analyzer.cpp syntax_analyzer.cpp executer.cpp lexem_vector.cpp lexems.cpp -o postfix $(CFLAGS)

clean:
	rm -rf *.o postfix

	

