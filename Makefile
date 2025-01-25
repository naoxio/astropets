CC=gcc
CFLAGS=-Wall -I./include -I./extern/cgltf
LIBS=-lglfw -lGL -lGLEW -lcglm -lm  # Added -lm for the math library

SOURCES=src/main.c src/shader_loader.c src/model_loader.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=alien_egg

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)