CFLAGS  = -g
_DEPS	= parser.h
ODIR	= obj
_OBJ	= main.o parser.o
OBJ	= $(patsubst %,$(ODIR)/%,$(_OBJ))

all: powderlog

%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(CFLAGS)

powderlog: main.o parser.o
	g++ -o $@ $^ $(CFLAGS)

test: test.o parser.o 
	g++ -o $@ $^ $(CFLAGS)

clean:
	-@rm -vf *.o
