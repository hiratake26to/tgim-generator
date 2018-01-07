PROGRAM = tgim-ns3

SRC_DIR = ./src/ ./src/ns3gen/ ./test/
SOURCES = $(wildcard $(addsuffix *.cpp,$(SRC_DIR)))

JSRC_DIR = ./local/
JSRC_LIST = $(wildcard $(addsuffix *.json,$(JSRC_DIR)))

#CXX 	= /usr/bin/g++
#CC 	= /usr/bin/gcc
OBJS 	= main.o $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
CFLAGS  = -I./include -g
CXXFLAGS  = -std=c++11 -I./include -I./thirdparty/include -I./thirdparty/PEGTL-2.2.0/include -g
VPATH 	= ./include/ $(SRC_DIR)

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $^ -lboost_program_options -lboost_filesystem -lboost_system


.PHONY: clean debug doc gen gen-debug

clean:
	rm -f $(PROGRAM) $(OBJS)

debug:
	make;
	./$(PROGRAM) --debug

doc:
	doxygen ./doc/Doxyfile

gen: $(JSRC_LIST)
	./init.sh;:
	./$(PROGRAM) $^

gen-debug: $(JSRC_LIST)
	cgdb --args ./$(PROGRAM) --input-file $^
