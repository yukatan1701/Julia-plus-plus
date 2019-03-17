CC=g++
CFLAGS=-c -std=c++11 -fmax-errors=5
LDFLAGS=
SOURCES=main.cpp lexical_analyzer.cpp syntax_analyzer.cpp executer.cpp lexem_vector.cpp lexems.cpp errors.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=postfix

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

	

