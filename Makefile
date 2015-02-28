CXXFLAGS=-W -Wall

all: motorcontrol

clean:
	rm -f motorcontrol

motorcontrol: motorcontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: all clean
