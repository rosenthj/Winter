#If you have clang, it seems to generate a faster compile as of the beginning of 2018
#CC=g++
CC=clang++
CFLAGS=-c -DNDEBUG -O3 -flto -Wall -Wextra -Wshadow -Wno-unused-parameter -m64 -msse4.2 -std=c++11 -Isrc -Isrc/general -Isrc/learning
LDFLAGS=-flto -Wall
SOURCES=$(wildcard src/general/*.cc src/learning/*.cc src/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
EXE:=Winter

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lpthread

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean: clean-src clean-general clean-learning

clean-src:
	rm -rf src/*.o
	
clean-general:
	rm -rf src/general/*.o
	
clean-learning:
	rm -rf src/learning/*.o
