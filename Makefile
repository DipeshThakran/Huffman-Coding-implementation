CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
TARGET := huffman

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

run: all
	./$(TARGET)