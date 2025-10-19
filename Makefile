CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lreadline

SRC = src/execute.c src/shell.c src/main.c
OBJ = obj/execute.o obj/shell.o obj/main.o
TARGET = bin/myshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Declare phony targets
.PHONY: all clean run

