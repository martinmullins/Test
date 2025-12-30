# Makefile for xextool

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = xextool
SRC = xextool.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "Build complete: $(TARGET)"

clean:
	rm -f $(TARGET)
	@echo "Clean complete"

# Help target
help:
	@echo "Available targets:"
	@echo "  all    - Build xextool (default)"
	@echo "  clean  - Remove built binary"
	@echo "  help   - Show this help message"
