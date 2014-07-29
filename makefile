BOOST=C:/Program Files/boost/boost_1_55_0
	
CC=clang++
CFLAGS=-O3 -c -std=c++1y
LDFLAGS=-L"$(BOOST)/stage/lib"

SOURCES=$(wildcard *.cpp) $(wildcard extensions/*/*.cpp)


INCLUDE=-I./include -I./extensions -I"$(BOOST)"

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=md-corrupter

all: $(SOURCES) $(EXECUTABLE) cleanobj

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

cleanobj:
	rm -rf $(OBJECTS)

rebuild: clean $(EXECUTABLE) cleanobj
