TARGETS =mainfile

.PHONY: clean

all: $(TARGETS)

%.o: %.c
	gcc -c -o $@ $<

mainfile: prompt.o functions.o start.o main.o ls.o signal.o
	gcc -o mainfile prompt.o functions.o start.o ls.o signal.o main.o -lncurses

clean:
	rm -f $(TARGETS) *.o

