CC = gcc

CFLAGS = -Wall -Wextra -O2 -Isrc

BUILD_DIR = build

TARGET1 = chess1
TARGET2 = chess2

COMMON_SRC = $(shell find src -name "*.c" \
	! -path "src/main.c" \
	! -path "src/main2.c" \
	! -path "src/game/game.c" \
	! -path "src/game/game2.c" \
	! -path "src/challenge/challenge.c" \
	! -path "src/challenge/challenge2.c")

CHALLENGE1_SRC = \
	src/main.c \
	src/game/game.c \
	src/challenge/challenge.c

CHALLENGE2_SRC = \
	src/main2.c \
	src/game/game2.c \
	src/challenge/challenge2.c

CHALLENGE1_OBJ = $(COMMON_SRC:src/%.c=$(BUILD_DIR)/challenge1/%.o) \
	$(CHALLENGE1_SRC:src/%.c=$(BUILD_DIR)/challenge1/%.o)

CHALLENGE2_OBJ = $(COMMON_SRC:src/%.c=$(BUILD_DIR)/challenge2/%.o) \
	$(CHALLENGE2_SRC:src/%.c=$(BUILD_DIR)/challenge2/%.o)

.PHONY: all challenge1 challenge2 clean

all: challenge1 challenge2

challenge1: $(TARGET1)

challenge2: $(TARGET2)

$(TARGET1): $(CHALLENGE1_OBJ)
	$(CC) $(CHALLENGE1_OBJ) -o $(TARGET1)

$(TARGET2): $(CHALLENGE2_OBJ)
	$(CC) $(CHALLENGE2_OBJ) -o $(TARGET2)

$(BUILD_DIR)/challenge1/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/challenge2/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET1) $(TARGET2)
