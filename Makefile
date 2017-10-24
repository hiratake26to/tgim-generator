PROGRAM = a.out
OBJS 	= main.o manager.o pool.o network.o NetUnit.o NetSeg.o InternetStack.o Link.o
CFLAGS  = -I./include
CXXFLAGS  = -std=c++11 -I./include
VPATH 	= ./include/ ./src/

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $^

clean:
	rm -f $(OBJS)

test:
	make clean; make;
	./$(PROGRAM) --debug
