#If you have clang, it seems to generate a faster compile as of the beginning of 2018
#CXX=g++

all:
ifeq ($(ARCH), aarch64)
CXX=aarch64-linux-android21-clang++
CFLAGS=-c -DNDEBUG -D_ARM -O3 -flto -Wall -Wno-sign-compare -m64 -march=armv8-a+fp+simd+crypto+crc -std=c++14 -Isrc -Isrc/general -Isrc/learning
LDFLAGS= -flto -Wall -pie -lm -static-libstdc++
else ifeq ($(ARCH), armv7a)
CXX=armv7a-linux-androideabi16-clang++
CFLAGS=-c -DNDEBUG -D_ARM -O3 -flto -Wall -Wno-sign-compare -m32 -mthumb -mfloat-abi=softfp -march=armv7-a -mfpu=neon -std=c++14 -Isrc -Isrc/general -Isrc/learning
LDFLAGS= -flto -Wall -pie -lm -static-libstdc++
else
CXX=clang++
CFLAGS=-c -DNDEBUG -O3 -flto -Wall -Wno-sign-compare -m64 -march=native -std=c++14 -Isrc -Isrc/general -Isrc/learning  
LDFLAGS= -flto -Wall -lpthread
endif
SOURCES=$(wildcard src/general/*.cc src/learning/*.cc src/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
EXE:=Winter

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS) 
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS)

.cc.o:
	$(CXX) $< -o $@ $(CFLAGS)

clean: clean-src clean-general clean-learning

clean-src:
	rm -rf src/*.o
	
clean-general:
	rm -rf src/general/*.o
	
clean-learning:
	rm -rf src/learning/*.o
