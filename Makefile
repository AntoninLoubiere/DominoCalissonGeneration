CXX=g++
CXXFLAGS=-Wall -O3
LDFLAGS=-Wall -O3
TARGET=dominos

all: main.o grid.o weighted_grid.o
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf *.o
