

all:
	g++ -Wall -ggdb3 -o encoder encoder.c -lm

clean:
	rm -f encoder
.PHONY: clean



