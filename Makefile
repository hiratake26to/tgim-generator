PROGRAM = ./bin/tgim-generator

SRC_DIR = ./src/ ./src/ns3gen/ ./test/
SOURCES = $(wildcard $(addsuffix *.cpp,$(SRC_DIR)))

JSRC_DIR = ./local/
JSRC_LIST = $(wildcard $(addsuffix *.json,$(JSRC_DIR)))

#CXX 	= /usr/bin/g++
#CC 	= /usr/bin/gcc
OBJS 	= main.o $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
DEBUG   = -g
#DEBUG   = -g3
CXX     = clang++
CFLAGS  = -I./include $(DEBUG)
CXXFLAGS  = -std=c++1z -I./include -I./thirdparty/include -I./thirdparty/PEGTL-2.2.0/include $(DEBUG)
VPATH 	= ./include/ $(SRC_DIR)

$(PROGRAM): $(OBJS)
	mkdir -p ./bin
	$(CXX) -o $(PROGRAM) $^ -lboost_program_options -lboost_filesystem -lboost_system


.PHONY: clean debug doc gen gen-debug tag

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
	cgdb --args ./$(PROGRAM) $^

tag:
	ctags -R
	gtags
