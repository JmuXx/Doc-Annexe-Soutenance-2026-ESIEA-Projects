CC = gcc

CFLAGS = -Wall -Wextra -O2 -I../src

BUILD_DIR = build

TARGET = $(BUILD_DIR)/test_runner


TEST_SRC = $(wildcard *.c)

SRC = $(shell find ../src -name "*.c" ! -name "main.c")

TEST_OBJ = $(TEST_SRC:%.c=$(BUILD_DIR)/%.o)

SRC_OBJ = $(SRC:../src/%.c=$(BUILD_DIR)/%.o)

OBJ = $(TEST_OBJ) $(SRC_OBJ)


all: $(TARGET)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)


$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/%.o: ../src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


run: all
	./$(TARGET)


clean:
	rm -rf $(BUILD_DIR)
