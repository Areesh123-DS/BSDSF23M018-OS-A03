CC = gcc
CFLAGS = -Iinclude -Wall

SRC = src/execute.c src/shell.c src/main.c
OBJ = obj/execute.o obj/shell.o obj/main.o
TARGET = bin/myshell

all: $(TARGET)

$(TARGET): $(OBJ)
<<<<<<< HEAD
	$(CC) $(OBJ) -o $(TARGET)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
run: $(TARGET)
	./$(TARGET)
=======
        $(CC) $(OBJ) -o $(TARGET)

obj/%.o: src/%.c
        $(CC) $(CFLAGS) -c $< -o $@

clean:
        rm -f $(OBJ) $(TARGET)
run: $(TARGET)
        ./$(TARGET)
>>>>>>> 5f705a10cdc9f4b8896b0387c2d5ea6cf59fc0ab

# Declare phony targets
.PHONY: all clean run
