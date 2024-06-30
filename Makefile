CXX=g++
CXXFLAGS=-Wall -O3
LDFLAGS=
TARGET=dominos

all: main.o grid.o
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf *.o
