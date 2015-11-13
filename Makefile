# Linux makefile for CS 354 skeleton code

CC= clang++
CFLAGS+= -std=c++11
LIBS= -lGL -L./lib -lglui -lGLU -lglut 
PROD= gasket

HDRS= $(shell ls *.c)
SRCS= $(shell ls *.c)
OBJS= $(patsubst %.c, %.o, $(SRCS))

all: $(PROD)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

$(PROD): $(OBJS)
	$(CC) -o $(PROD) $^ $(LIBS)

clean:
	rm -f $(PROD)
	rm -f *.o

