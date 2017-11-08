PROGRAM = a.out

SRC_DIR = ./src/ ./src/ns3gen/ ./test/
SOURCES = $(wildcard $(addsuffix *.cpp,$(SRC_DIR)))

OBJS 	= main.o $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
CFLAGS  = -I./include -g
CXXFLAGS  = -std=c++11 -I./include -g
VPATH 	= ./include/ $(SRC_DIR)

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $^


.PHONY: clean debug run lex yacc

clean:
	rm -f $(PROGRAM) $(OBJS)

debug:
	echo $(OBJS)

run:
	make;
	./$(PROGRAM) --debug

lex: src/tmgr.l
	yacc -d src/tmgr.y
	flex $<

yacc: src/tmgr.y lex
	yacc -d $<
	$(CC) y.tab.c
