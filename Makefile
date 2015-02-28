CXXFLAGS=-g -W -Wall

all: motorcontrol

clean:
	rm -f motorcontrol

motorcontrol: motorcontrol.o scchk.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: all clean
