# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Target executable
TARGET = $(BIN_DIR)/app

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean
