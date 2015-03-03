CXXFLAGS=-g -W -Wall

all: motorcontrol

clean:
	rm -f motorcontrol *.o

motorcontrol: motorcontrol.o dcdriver.o scchk.o packetreader.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: all clean
