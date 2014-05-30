
all:
	g++  -Wall -ggdb3 -o encoder encoder.cpp -lm `Magick++-config --cxxflags --cppflags --ldflags --libs`

clean:
	rm -f encoder
.PHONY: clean



