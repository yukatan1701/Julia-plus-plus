CC=g++
CFLAGS=-I include -std=c++11 -fmax-errors=5
LDFLAGS=
SRCDIR=sources
BINDIR=bin
FILES=julia.cpp lexical_analyzer.cpp syntax_analyzer.cpp executer.cpp lexem_vector.cpp lexems.cpp errors.cpp
SOURCES=$(FILES:%.cpp=$(SRCDIR)/%.cpp)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
EXECUTABLE=julia

all: $(SOURCES) $(EXECUTABLE)

lib: $(OBJECTS)
	rm -f julia.a
	ar rvs julia.a $(OBJECTS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) main.cpp -o $@

$(OBJECTS): $(BINDIR)/%.o : $(SRCDIR)/%.cpp | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf *.o $(EXECUTABLE)

	

