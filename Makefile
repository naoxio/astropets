CC=gcc
CFLAGS=-Wall -I/opt/raylib/include
LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Directories
SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin

# Source files and object files
SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
EXECUTABLE=$(BIN_DIR)/digipets

# Create necessary directories
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BIN_DIR))

# Main target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean