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

APPNAME1 := counter
APPNAME2 := mysort

CFILES :=  zutils.cpp  sensebarrier.cpp  taslock.cpp ttaslock.cpp ticketlock.cpp mcslock.cpp petersonlock.cpp petersonrellock.cpp 
CFILES1 := counter.cpp $(CFILES)
CFILES2 := mysort.cpp bucketsort.cpp $(CFILES)

OBJFILES1 := $(CFILES1:.cpp=.o)
OBJFILES2 := $(CFILES2:.cpp=.o)

all: $(APPNAME1) $(APPNAME2)

$(APPNAME1): $(OBJFILES1) 
	$(CC) -o $(APPNAME1) $(CFLAGS) $^ 
	@echo "LINK => $@"

$(APPNAME2): $(OBJFILES2) 
	$(CC) -o $(APPNAME2) $(CFLAGS) $^ 
	@echo "LINK => $@"


%.o: %.cpp
	$(CC) -c $< -o $@  $(CFLAGS)
	
.PHONY: clean
clean:
	rm *.o
