CC=gcc
CFLAGS=-Wall -I/opt/raylib/include
LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SOURCES=src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=alien_egg

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)