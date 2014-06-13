
all:
	g++  -Wall -ggdb3 -o encoder.o encoder.cpp -lm `Magick++-config --cxxflags --cppflags --ldflags --libs`
	g++ -Wall -ggdb3 -o decoder.o decoder.cpp -lm `Magick++-config --cxxflags --cppflags --ldflags --libs`
clean:
	rm -f *.o
.PHONY: clean



