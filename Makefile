CXX=g++
CXXFLAGS=-Wall -O3
LDFLAGS=-Wall -O3
TARGET=dominos

all: main.o grid.o weighted_grid.o
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

calisson: calisson.o grid.o weighted_grid.o
	$(CXX) -o calisson $^ $(LDFLAGS)

clean:
	rm -rf *.o
