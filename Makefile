CFLAGS  = -g
_DEPS	= parser.h
ODIR	= obj
_OBJ	= main.o parser.o
OBJ	= $(patsubst %,$(ODIR)/%,$(_OBJ))


%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(CFLAGS)

main: main.o parser.o
	g++ -o $@ $^ $(CFLAGS)

test: test.o parser.o 
	g++ -o $@ $^ $(CFLAGS)

clean:
	-@rm -vf *.o
