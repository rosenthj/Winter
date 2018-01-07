#If you have clang, it seems to generate a faster compile as of the beginning of 2018
CC=g++
#CC=clang++
CFLAGS=-c -DNDEBUG -Ofast -flto -g3 -Wall -Wno-sign-compare -march=native -m64 -std=c++11 -Isrc -Isrc/general -Isrc/learning
LDFLAGS=-flto -Wall
SOURCES=$(wildcard src/general/*.cc src/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=Winter

all: $(SOURCES) $(EXECUTABLE) clean

target no_bmi: CFLAGS += -DNO_BMI
no_bmi: all
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lpthread

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean: clean-src clean-general

clean-src:
	rm -rf src/*.o
	
clean-general:
	rm -rf src/general/*.o
