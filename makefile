CC=g++
CFLAGS=

vpath %.cpp share
vpath %.h include

all: main.o taslock.o zutils.o ttaslock.o ticketlock.o \
      mcslock.o
	$(CC) -o main -g -pthread -O3 -std=c++2a $^

main.o: main.cpp zutils.h taslock.h ttaslock.h ticketlock.h
	$(CC) -g -pthread -O3 -std=c++2a -c $< -o $@

zutils.o: zutils.cpp zutils.h
	$(CC) -g -std=c++2a -c $< -o $@
  
taslock.o: taslock.cpp taslock.h
	$(CC) -g -std=c++2a -c $< -o $@

ttaslock.o: ttaslock.cpp ttaslock.h
	$(CC) -g -std=c++2a -c $< -o $@

ticketlock.o: ticketlock.cpp ticketlock.h
	$(CC) -g -std=c++2a -c $< -o $@

mcslock.o: mcslock.cpp mcslock.h
	$(CC) -g -std=c++2a -c $< -o $@

.PHONY: clean
clean:
	rm *.o
