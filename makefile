OS:=$(shell uname -s)

ifeq ($(OS), Darwin)
	CC := clang
	CFLAGS := -g -pthread -lstdc++ -O3 -std=c++2a
endif

ifeq ($(OS), Linux)
	CC := c++
    CFLAGS := -g -pthread -O3 -std=c++2a
endif

vpath %.cpp share
vpath %.h include

APPNAME := main

CFILES := main.cpp senserelbarrier.cpp  zutils.cpp taslock.cpp ttaslock.cpp ticketlock.cpp mcslock.cpp petersonlock.cpp petersonrellock.cpp 

OBJFILES := $(CFILES:.cpp=.o)

all: $(APPNAME)

$(APPNAME): $(OBJFILES) 
	$(CC) -o $(APPNAME) $(CFLAGS) $^ 
	@echo "LINK => $@"


%.o: %.cpp
	$(CC) -c $< -o $@  $(CFLAGS)
	
.PHONY: clean
clean:
	rm *.o
