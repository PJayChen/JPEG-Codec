
all:
	g++  -Wall -ggdb3 -o encoder encoder.c -lm `Magick++-config --cxxflags --cppflags --ldflags --libs`

clean:
	rm -f encoder
.PHONY: clean



