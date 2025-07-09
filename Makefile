# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

#Automatically find all .c files in current directory
SRC := $(wildcard *.c)

# Replace .c with .o to get object file list
OBJ := $(SRC:.c=.o)

LIBS = -lncurses

# Final executable name
TARGET = wm_file_manager

# Default build rule
all: $(TARGET)

# Link all object files to create the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)

# Pattern rule to compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

# Make the program a systemwide binary, use sudo along.
install:
	cp $(TARGET) /usr/local/bin/$(TARGET)
